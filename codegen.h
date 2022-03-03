#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct instruction
{
	int op; // Opcode
	int l; 	// Lexografical level
	int m;	// M
}instruction;

instruction *runCodeGen(symbol *table, token *list);

#endif
