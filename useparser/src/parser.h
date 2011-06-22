#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "binary.h"
#include "overview.h"

void parser_startup();
void parser_process(char *line);
void parser_shutdown();

#endif /* _PARSER_H */

