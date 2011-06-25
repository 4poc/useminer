#include "binary.h"

binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total)
{
    binary_t *binary;

    if(num > total) {
        ERROR("num > total! subj: %s\n", overview.subject);
        return NULL;
    }

    //DEBUG("new binary num/total: %d/%d\n", num, total);

    binary = malloc(sizeof(binary_t));
    if(!binary) {
        ERROR("unable to allocate memory for binary_t");
        return NULL;
    }

    binary->subject = copy_string(overview.subject);
    binary->from = copy_string(overview.from);

    /* parse date */
    binary->date = parse_date(overview.date);

    /* parse xref for newsgroups, and append, I think that
     * the group where the xzver data comes from is included in xref,
     * not really sure through */
    binary->newsgroups = NULL;
    if(overview.xref) {
        binary->newsgroups = parse_xref(overview.xref);
    }
    else {
        ERROR("overview struct missing xref!\n");
    }

    binary->parts_completed = 1;
    binary->parts_total = total;
    /* allocate for all parts */
    binary->parts = malloc(sizeof(binary_part_t*) * total);
    if(!binary->parts) {
        ERROR("unable to allocate memory for binary_t.parts");
        return NULL;
    }
    //DEBUG("binary->parts[%d] allocated %p\n",
    //       sizeof(binary_part_t*) * total, binary->parts);
    memset(binary->parts, 0, sizeof(binary_part_t*) * total);
    if(num >= 1 && num <= total) { /* this would be an invalid article otherwise */
        binary->parts[num-1] = new_binary_part(
                overview.message_id, atoi(overview.bytes));
    }
    /* else { free binary; return NULL; } */

    return binary;
}

void insert_binary_part(binary_t *binary, overview_t overview, uint16_t num)
{
    if(num > binary->parts_total) {
        ERROR("error try to insert part num > total!\n");
        ERROR("overview.subject: %s\n", overview.subject);
        ERROR("binary->subject: %s\n", binary->subject);
        return;
    }


    //DEBUG("insert binary at %p %d/%d\n", binary, num, binary->parts_total);
    //DEBUG("binary->parts at %p\n", binary->parts)
    DEBUG("parts[%d]/total:%d\n", num-1, binary->parts_total);
    if(binary->parts[num-1] != NULL) {
        /* the part specified by num is already set. I'm unsure about how
         * to handle this case, most indexer seem to let newer parts
         * overwrite the old ones.
         * I'm not really sure why this is happening, the message_id is 
         * different in most cases but not the contents? What if the binary 
         * is already completed and written away?
         * 
         * So for now I let new parts overwrite the old ones... */
        DEBUG("binary part number already there, overwrite (%s)\n",
                overview.subject);
        free_binary_part(binary->parts[num-1]);
    }

    binary->parts[num-1] = 
        new_binary_part( overview.message_id, atoi(overview.bytes)); 
    binary->parts_completed++;
}

void free_binary(binary_t *binary)
{
    if(binary == NULL) {
        return;
    }

    FREE(binary->subject);
    FREE(binary->from);

    free_newsgroup(binary->newsgroups);

    for(int i=0; i<binary->parts_total;i++){
        if(binary->parts[i]) {
            free_binary_part(binary->parts[i]);
        }
    }
    FREE(binary->parts);

    FREE(binary);
}

binary_part_t *new_binary_part(char *message_id, uint32_t bytes)
{
    DEBUG("new_binary_part(%s, %d);\n", message_id, bytes);
    binary_part_t *binary_part;
    binary_part = malloc(sizeof(binary_part_t));
    if(!binary_part) {
        ERROR("unable to allocate memory for binary_part_t");
        return NULL;
    }
    binary_part->message_id = copy_string(message_id);
    binary_part->bytes = bytes;
    DEBUG("allocated binary_part: %p\n", binary_part);
    return binary_part;
}

void free_binary_part(binary_part_t *part)
{
    DEBUG("free_binary_part(%p);\n", part);
    if(!part || !(part->message_id)) {
        return;
    }
    FREE(part->message_id);
    FREE(part);
}

bool complete_binary(binary_t *binary)
{
    return binary->parts_completed == binary->parts_total;
}

