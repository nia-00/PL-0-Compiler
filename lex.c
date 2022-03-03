// Estefania Sanchez
// es102053
// COP 3402, Summer 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"

#define LEX_NO_ERROR -1

const char *reservedWords[] = {"const", "var", "procedure", "call", "if", "then",
 															 "else", "while", "do", "read", "write", "odd",
															 "begin", "end"};
const char specialSymbols[] = {'+', '-', '*', '/', '(', ')', '=', ',',
                                '.', '<', '>', ';', ':'};
int tokenIndex = 0, errorType;
token *tokenList;

char* filename;


//prints out errors
void error(int errorCase)
{
		errorType = errorCase;
    switch (errorCase)
		{
        case 1:
            printf("***Error 1: Use = instead of :=.\n");
            break;
        case 2:
            printf("***Error 2: = must be followed by a number.\n");
            break;
        case 3:
            printf("***Error 3: Identifier must be followed by =.\n");
            break;
        case 4:
            printf("***Error 4: const, var, procedure must be followed by identifier.\n");
            break;
        case 5:
            printf("***Error 5: Semicolon or comma missing.\n");
            break;
        case 6:
            printf("***Error 6: Incorrect symbol after procedure declaration.\n");
            break;
/*
Errors 7, 8 and 10 are deprecated
        case 7:
            printf("***Error 7: Statement expected\n");
            break;
        case 8:
            printf("***Error 8: Incorrect symbol after statement part in block.\n");
            break;
        case 10:
            printf("***Error 10: Semicolon between statements missing.\n");
            break;
*/
        case 9:
            printf("***Error 9: Period expected.\n");
            break;
        case 11:
            printf("***Error 11: Undeclared identifier.\n");
            break;
        case 12:
            printf("***Error 12: Assignment to constant or procedure is not allowed.\n");
            break;
        case 13:
            printf("***Error 13: Assignment operator expected.\n");
            break;
        case 14:
            printf("***Error 14: Call must be followed by an identifier\n");
            break;
        case 15:
            printf("***Error 15: Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("***Error 16: then expected\n");
            break;
/*  Error 17 was depricated
        case 17:
            printf("***Error 17: Semicolon or end expected\n");
            break;
*/
        case 18:
            printf("***Error 18: do expected\n");
            break;
/* Error 19 was depricated
        case 19:
            printf("***Error 19: Incorrect symbol following statement.\n");
            break;
*/
        case 20:
            printf("***Error 20: Relational operator expected.\n");
            break;
        case 21:
            printf("***Error 21: Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("***Error 22: Right parenthesis missing.\n");
            break;
/* Errors 23 and 24 were depricated
        case 23:
            printf("***Error 23: The preceding factor cannot begin with this symbol.\n");
            break;
        case 24:
            printf("***Error 24: An expression cannot begin with this symbol.\n");
            break;
*/
        case 25:
            printf("***Error 25: This number is too large.\n");
            break;
				case 26:
						printf("***Error 26: Identifier too long\n");
						break;
				case 27:
						printf("***Error 27: Invalid symbol\n");
						break;
				case 28:
						printf("***Error 28: Variable does not start with letter.\n");
						break;
				case 29:
            printf("***Error 29: Program too long. Max code length = %d\n", MAX_CODE_LENGTH);
            break;
				case 30:
						printf("***Error 30: Symbol is unmarked in symbol table\n");
						break;
				case 31:
						printf("***Error 31: Code following a period\n");
						break;
        default:
            break;
    }
    exit(1);
}

// This function reads in the input file, puts all the characters into an array
// and prints the source code from the input file into the output file.
int printInput(FILE *ifp, FILE *ofp, char *sourceProgram)
{
  int i = 0;
  fprintf(ofp, "Source Program:\n");
  while (fscanf(ifp, "%c", &sourceProgram[i]) != EOF)
  {
    fprintf(ofp, "%c", sourceProgram[i]);
    i++;
  }
  fprintf(ofp, "\n");
  return i;
}

// This function takes the array of tokens and prints out the lexeme list.
void printTokenList(FILE *ofp, token *tokenList, int tokenIndex)
{
  int i;
  fprintf(ofp,"\n\nLexeme List:\n");
  for (i = 0; i < tokenIndex; i++)
  {
    fprintf(ofp, "%d ", tokenList[i].id);
    if (tokenList[i].id == 2)
    {
      fprintf(ofp, "%s ", tokenList[i].name);
    }
    else if (tokenList[i].id == 3)
    {
      fprintf(ofp, "%d ", tokenList[i].number);
    }
  }
  fprintf(ofp, "\n");
}

// This function takes the string found in the program, the reserved word it
// corresponds with and the token list and adds the correct integer into the
// token list.
void addReservedWord(int reservedWord, int tokenIndex, char *varBuffer, token *tokenList)
{
  switch (reservedWord)
  {
    case 0: // const
      tokenList[tokenIndex].id = constsym;
      return;
    case 1: // var
      tokenList[tokenIndex].id = varsym;
      return;
    case 2: // procedure
      tokenList[tokenIndex].id = procsym;
      return;
    case 3: // call
      tokenList[tokenIndex].id = callsym;
      return;
    case 4: // if
      tokenList[tokenIndex].id = ifsym;
      return;
    case 5: // then
      tokenList[tokenIndex].id = thensym;
      return;
    case 6: // else
      tokenList[tokenIndex].id = elsesym;
      return;
    case 7: // while
      tokenList[tokenIndex].id = whilesym;
      return;
    case 8: // do
      tokenList[tokenIndex].id = dosym;
      return;
    case 9: // read
      tokenList[tokenIndex].id = readsym;
      return;
    case 10: // write
      tokenList[tokenIndex].id = writesym;
      return;
		case 11: // odd
			tokenList[tokenIndex].id = oddsym;
			return;
		case 12: // begin
			tokenList[tokenIndex].id = beginsym;
			return;
		case 13: // end
			tokenList[tokenIndex].id = endsym;
			return;
    default: // variable name
      tokenList[tokenIndex].id = identsym;
      strcpy(tokenList[tokenIndex].name, varBuffer);
      return;
  }
}

// This function runs the lexical analyzer
token *runLex(char *filename)
{
  FILE *ifp = fopen(filename, "r");
  FILE *ofp = fopen("lexOutput.txt", "w");
  tokenList = malloc(MAX_CODE_LENGTH * sizeof(token));
  char sourceProgram[5000];
  int codeLength = printInput(ifp, ofp, sourceProgram);
  int programIndex = 0;
  int commentFlag = 0;
  int printFlag = 0;
  int peek = 0;
  int num;
  int i;
  int isReserved = -1;
  int isSpecial = -1;
  char varBuffer[12];
  char numBuffer[6];
  char symBuffer;
  char temp;

  fprintf(ofp, "\nLexeme Table:\nlexeme\t\ttoken type\n");
  // the while loop goes through the program array until the end of the program.
  while (programIndex < codeLength)
  {
    // we ignore the white space in the program
    if (isspace(sourceProgram[programIndex]))
    {
      programIndex++;
      peek = 0;
      continue;
    }
    // this checks for a string in the program
    else if (isalpha(sourceProgram[programIndex]))
    {
      i = 0;
      peek = 1;
      varBuffer[i] = sourceProgram[programIndex];
      i++;
      programIndex++;

      // this gets the word from the program
      while (isalpha(sourceProgram[programIndex]) || isdigit(sourceProgram[programIndex]))
      {
        // if the string is longer than 11 characters, we have an error
        if (i > 10)
        {
          fprintf(ofp, "Error: Identifier name too long.\n");
          error(26);
          return tokenList;
        }
        varBuffer[i] = sourceProgram[programIndex];
        i++;
        programIndex++;
      }
      varBuffer[i] = '\0';
      isReserved = -1;
      // check if the word is a reserved word
      for (i = 0; i < 14; i++)
      {
        if(strcmp(varBuffer, reservedWords[i]) == 0)
        {
          isReserved = i;
          break;
        }
      }
      // add the correct nteger into the current token id cell
      addReservedWord(isReserved, tokenIndex, varBuffer, tokenList);
      fprintf(ofp,"%s\t\t%d\n", varBuffer, tokenList[tokenIndex].id);
      tokenIndex++;
    }
    // this checks for a number in the program
    else if(isdigit(sourceProgram[programIndex]))
    {
      i = 0;
      peek = 1;
      numBuffer[i] = sourceProgram[programIndex];
      i++;
      programIndex++;
      // this gets the entire number
      while (isdigit(sourceProgram[programIndex]))
      {
        // if the number is longer than 5 digits we have an error
        if (i > 5)
        {
          fprintf(ofp, "Error: Number too long.\n");
          error(25);
          return tokenList;
        }
        numBuffer[i] = sourceProgram[programIndex];
        i++;
        programIndex++;
      }
      // if there is an alphabetical character after the numbers then we have a
      // variable that starts with a number, which is an error.
      if (isalpha(sourceProgram[programIndex]))
      {
        fprintf(ofp, "Error: Variable does not start with letter.\n");
        error(28);
        return tokenList;
      }
      numBuffer[i] = '\0';
      num = atoi(numBuffer);
      tokenList[tokenIndex].id = numbersym;
      tokenList[tokenIndex].number = num;
      fprintf(ofp,"%d\t\t%d\n", num, tokenList[tokenIndex].id);
      tokenIndex++;
    }
    // this covers special symbols in the source program
    else
    {
      peek = 0;
      isSpecial = -1;
      symBuffer = sourceProgram[programIndex];
      // this loop compares the current symbol to those in the specialSymbols
      // array and stores which one it is
      for (i = 0; i < 13; i++)
      {
        if(symBuffer == specialSymbols[i])
        {
          isSpecial = i;
          break;
        }
      }
      // this switch handles the different special symbols for the program.
      switch(isSpecial)
      {
        case 0: // +
          tokenList[tokenIndex].id = plussym;
          break;
        case 1: // -
          tokenList[tokenIndex].id = minussym;
          break;
        case 2: // *
          tokenList[tokenIndex].id = multsym;
          break;
        case 3: // /
          peek = 1;
          // this checks for comments in the code and skips over whatever is in
          // the comment
          if (sourceProgram[programIndex + 1] == '*')
          {
            printFlag = 1;
            programIndex += 2;
            commentFlag = 1;
            while (commentFlag != 0)
            {
              if (sourceProgram[programIndex] == '*' && sourceProgram[programIndex + 1] == '/')
              {
                commentFlag = 0;
                programIndex++;
              }
              programIndex++;
            }
          }
          else
          {
            tokenList[tokenIndex].id = slashsym;
            fprintf(ofp,"%c\t\t%d\n", sourceProgram[programIndex], tokenList[tokenIndex].id);
            tokenIndex++;
            printFlag = 1;
          }
          programIndex++;
          break;
        case 4: // (
          tokenList[tokenIndex].id = lparentsym;
          break;
        case 5: // )
          tokenList[tokenIndex].id = rparentsym;
          break;
        case 6: // =
          tokenList[tokenIndex].id = eqsym;
          break;
        case 7: // ,
          tokenList[tokenIndex].id = commasym;
          break;
        case 8: // .
          tokenList[tokenIndex].id = periodsym;
          break;
        case 9: // <
          if (sourceProgram[programIndex + 1] == '>')
          {
            tokenList[tokenIndex].id = neqsym;
            fprintf(ofp,"%c%c\t\t%d\n", sourceProgram[programIndex], sourceProgram[programIndex + 1], tokenList[tokenIndex].id);
            programIndex += 2;
            tokenIndex++;
            printFlag = 1;
          }
          // this checks for <= symbol
          else if (sourceProgram[programIndex + 1] == '=')
          {
            tokenList[tokenIndex].id = leqsym;
            fprintf(ofp,"%c%c\t\t%d\n", sourceProgram[programIndex], sourceProgram[programIndex + 1], tokenList[tokenIndex].id);
            programIndex += 2;
            tokenIndex++;
            printFlag = 1;
          }
          else
          {
            tokenList[tokenIndex].id = lessym;
          }
          break;
        case 10: // >
          // this checks for >= symbol
          if (sourceProgram[programIndex + 1] == '=')
          {
            tokenList[tokenIndex].id = geqsym;
            fprintf(ofp,"%c%c\t\t%d\n", sourceProgram[programIndex], sourceProgram[programIndex + 1], tokenList[tokenIndex].id);
            tokenIndex++;
            programIndex += 2;
            printFlag = 1;
          }
          else
          {
            tokenList[tokenIndex].id = gtrsym;
          }
          break;
        case 11: // ;
          tokenList[tokenIndex].id = semicolonsym;
          break;
        case 12: // :
          // this checks for := symbol
          if (sourceProgram[programIndex + 1] == '=')
          {
            tokenList[tokenIndex].id = becomessym;
            fprintf(ofp,"%c%c\t\t%d\n", sourceProgram[programIndex], sourceProgram[programIndex + 1], tokenList[tokenIndex].id);
            tokenIndex++;
            programIndex += 2;
            printFlag = 1;
          }
          // if there is no = after a :, then we have an invalid symbol
          else
          {
            fprintf(ofp, "Error: Invalid symbols.\n");
            error(27);
            return tokenList;
          }
          break;
        default: // this takes care of symbols not recognized by the program
          fprintf(ofp, "Error: Invalid symbols.\n");
          error(27);
          return tokenList;
      }

      // if the token and id have already been printed, this makes it so we don't
      // print it twice
      if (printFlag == 1)
      {
        printFlag = 0;
        continue;
      }
      fprintf(ofp,"%c\t\t%d\n", sourceProgram[programIndex], tokenList[tokenIndex].id);
      tokenIndex++;
    }
    // if we have not moved the programIndex forward, then increase it
    if (peek == 0)
    {
      programIndex++;
    }
  }
  // This function prints the lexeme list
  printTokenList(ofp, tokenList, tokenIndex);

  fclose(ifp);
  fclose(ofp);

  return tokenList;
}
