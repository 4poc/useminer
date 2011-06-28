#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "binary.h"
#include "overview.h"

#include "storage.h"


bool parser_init();
void parser_uninit();

void parser_process(char *line);

int parser_complete_count();


#endif /* _PARSER_H */

