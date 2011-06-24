#include "binary.h"

binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total)
{
    binary_t *binary;

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
    DEBUG("binary->parts[%d] allocated %p\n",
           sizeof(binary_part_t*) * total, binary->parts);
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
    //DEBUG("insert binary at %p %d/%d\n", binary, num, binary->parts_total);
    //DEBUG("binary->parts at %p\n", binary->parts)
    if(binary->parts[num-1] != NULL) {
        ERROR("binary part number already there!, skipping. (%s (%d))\n",
                overview.subject, num);
        return;
    }

    binary->parts[num-1] = new_binary_part(
            overview.message_id, atoi(overview.bytes));
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
    binary_part_t *binary_part;
    binary_part = malloc(sizeof(binary_part));
    if(!binary_part) {
        ERROR("unable to allocate memory for binary_part_t");
        return NULL;
    }
    binary_part->message_id = copy_string(message_id);
    binary_part->bytes = bytes;
    return binary_part;
}

void free_binary_part(binary_part_t *part)
{
    if(!part) {
        return;
    }
    FREE(part->message_id);
    FREE(part);
}

bool complete_binary(binary_t *binary)
{
    return binary->parts_completed == binary->parts_total;
}

