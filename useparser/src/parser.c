#include "parser.h"

char *hash_data, *hash=NULL;
overview_t overview;
uint16_t num, total;

bool parser_init()
{
    DEBUG("parser startup\n");
    hash = NULL;

    if(!storage_init()) {
        return false;
    }

    return true;
}

void parser_uninit()
{
    DEBUG("parser shutdown\n");
    FREE(hash);

    storage_uninit();
}

void parser_process(char *line)
{
    binary_t *binary;

    /* splice line into the headers of overview_t */
    overview = parse_overview(line);
    
    /* parse subject line for multipart numbers (num and total) */
    if(!parse_subject(overview.subject, &num, &total)) {
        ERROR("article subject not valid multipart binary? (subject:%s)",
                overview.subject);
        return;
    }

    /* calculate hash for subject and from headers,
     * (NOTE: subject is spliced from num and total by parse subject) */
    hash_data = join_string(overview.subject, overview.from);
    md5(hash_data, strlen(hash_data), &hash);
    FREE(hash_data);

    // if(binary = storage_search(hash) != NULL) {
        /* binary found in storage, add new binary part */
    //     insert_binary_part(binary, overview, num);
    // }
    // else { */ /* this part is the first of this binary */

        /* create new binary */
        binary = new_binary(overview, num, total);
        if(!binary) {
            ERROR("error creating new binary?! (subject:%s)\n", overview.subject);
            return;
        }
        storage_new(hash, binary);

    /* } */

    //if(complete_binary(binary)) { /* all parts of binary completed */
        /* append tupel for this binary to postgres binary COPY file */
        /* remove from storage */
        //storage_remove(hash);
    //}

    free_binary(binary);
}

