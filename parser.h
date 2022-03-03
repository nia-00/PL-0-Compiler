#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

#define MAX_SYMBOL_TABLE_SIZE 500

typedef struct
{
	int kind;					// const = 1, var = 2, proc = 3
	char name[12];		// name up to 11 chars
	int val;					// number (ASCII value)
	int level;				// L level
	int addy;					// M address
	int mark;					// to indicate unavailable or delete d
} symbol;

int tableIndex;

symbol *runParse(token *tokenList);

#endif
