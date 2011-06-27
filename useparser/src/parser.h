#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "binary.h"
#include "overview.h"

#include "storage.h"


bool parser_init();
void parser_uninit();

void parser_process(char *line);

int get_completed_binary();


#endif /* _PARSER_H */

