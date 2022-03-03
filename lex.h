#ifndef __LEX_H
#define __LEX_H

#define MAX_CODE_LENGTH 500

typedef enum
{
  nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, oddsym,
  eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
  semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym,
  dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;

typedef struct
{
  token_type id;
  int number;
  char name[12];
}token;

token *runLex(char *filename);
void error(int errorCase);

extern int errorType;
extern int tokenIndex;
extern char *filename;

#endif
