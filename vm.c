// Estefania Sanchez
// es102053
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

// The following constants are set strings and lengths specified by the assignment.
const char *OPCODES[] = {"NULL", "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio", "sio"};

FILE *ofp2;
FILE *ofp3;

// This function finds the activation record l levels down
int base(int *stack, int l, int bp)
{
	int goal = bp;
	while (l > 0)
	{
		goal = stack[goal - l];
		l--;
	}

	return goal;
}

// This function will print out the list of instructions in the input file in the
// order given in the file.
void printInstructions(instruction *instructionsArray, FILE *ofp)
{
	int i = 0;
	fprintf(ofp, "Line\tOP\tL\tM\n");
	fprintf(ofp2, "Line\tOP\tL\tM\n");
	fflush(stdout);
	while(1)
	{
		fprintf(ofp, "%d\t%s\t%d\t%d\n", i, OPCODES[instructionsArray[i].op], instructionsArray[i].l, instructionsArray[i].m);
		fprintf(ofp2, "%d\t%s\t%d\t%d\n", i, OPCODES[instructionsArray[i].op], instructionsArray[i].l, instructionsArray[i].m);
		if (instructionsArray[i].op == 11 && instructionsArray[i].m == 3)
		{
			break;
		}
		i++;
	}
}

// This function will print out the stack and activation records.
void printStack(FILE *ofp, int *stack, int bp, int sp)
{
	int i;
	if (bp > 999) // if we are not on the stack do not print
	{
		return;
	}
	// if the base pointer is not at the top of the stack, this means we may have to
	// print out activation records
	else if (bp < 999)
	{
		// recursively call the function to get to the top of the stack
		printStack(ofp, stack, stack[bp - 1], bp + 1);
		// if sp > bp that means the cal instruction was just executed and we do not
		// print the activation record.
		if(sp > bp)
			return;
			fprintf(ofp,"| ");
			fprintf(ofp3,"| ");
	}
	// print out the stack from the base pointer to the stack pointer.
	for (i = bp; i >= sp; i--)
	{
		fprintf(ofp, "%d ", stack[i]);
		fprintf(ofp3, "%d ", stack[i]);
	}
}

// This function reads the input file and puts all of the instructions into an
// an array of instructions
void fetchInstructions(char *filename, instruction *instructionsArray)
{
	FILE *ifp = fopen(filename, "r");
	int i = 0;
	while (fscanf(ifp, "%d %d %d", &instructionsArray[i].op, &instructionsArray[i].l, &instructionsArray[i].m) != EOF)
	{
		i++;
	}
	fclose(ifp);
}

// This function executes the instruction that is passed to it.
void executeStep(instruction *currentInstruction, int *stack, int *sp, int *bp, int *pc, int *haltFlag, FILE *ofp, FILE *ofp3)
{
		switch (currentInstruction->op)
		{
			case 1: // lit instruction
				*sp = *sp - 1;
				stack[*sp] = currentInstruction->m;
				break;
			case 2: // opr instruction
				switch (currentInstruction->m)
				{
					case 0: // ret instruction
						*sp = *bp + 1;
						*pc = stack[*sp - 4];
						*bp = stack[*sp - 3];
						break;
					case 1: // neg instruction
						stack[*sp] = -stack[*sp];
						break;
					case 2: // add instruction
						*sp = *sp + 1;
						stack[*sp] = stack[*sp] + stack[*sp - 1];
						break;
					case 3: // sub instruction
						*sp = *sp + 1;
						stack[*sp] = stack[*sp] - stack[*sp - 1];
						break;
					case 4: // mul instruction
						*sp = *sp + 1;
						stack[*sp] = stack[*sp] * stack[*sp - 1];
						break;
					case 5: // div instruction
						*sp = *sp + 1;
						stack[*sp] = stack[*sp] / stack[*sp - 1];
						break;
					case 6: // odd instruction
						stack[*sp] = stack[*sp] % 2;
						break;
					case 7: // mod instruction
						*sp = *sp + 1;
						stack[*sp] = stack[*sp] % stack[*sp - 1];
						break;
					case 8: // eql instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] == stack[*sp - 1]);
						break;
					case 9: // neq instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] != stack[*sp - 1]);
						break;
					case 10: // lss instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] < stack[*sp - 1]);
						break;
					case 11: // leq instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] <= stack[*sp - 1]);
						break;
					case 12: // gtr instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] > stack[*sp - 1]);
						break;
					case 13: // geq instruction
						*sp = *sp + 1;
						stack[*sp] = (stack[*sp] >= stack[*sp - 1]);
						break;
				}
				break;
			case 3: // lod instruction
				*sp = *sp - 1;
				stack[*sp] = stack[(base(stack, currentInstruction->l,*bp)) - (currentInstruction->m)];
				break;
			case 4: // sto instruction
				stack[(base(stack, currentInstruction->l,*bp)) - (currentInstruction->m)] = stack[*sp];
				*sp = *sp + 1;
				break;
			case 5: // cal instruction
				stack[*sp - 1] = 0;
				stack[*sp - 2] = base(stack, currentInstruction->l,*bp);
				stack[*sp - 3] = *bp;
				stack[*sp - 4] = *pc;
				*bp = *sp - 1;
				*pc = currentInstruction->m;
				break;
			case 6: // inc instruction
				*sp = *sp - currentInstruction->m;
				break;
			case 7: // jmp instruction
				*pc = currentInstruction->m;
				break;
			case 8: // jpc instruction
				if (stack[*sp] == 0)
					*pc = currentInstruction->m;
				*sp = *sp + 1;
				break;
			case 9: // sio 1 instruction
				fprintf(ofp,"%d\n", stack[*sp]);
				fprintf(ofp3,"%d\n", stack[*sp]);
				*sp = *sp + 1;
				break;
			case 10: // sio 2 instruction
				*sp = *sp - 1;
				printf("Input a number and press enter.\n");
				scanf("%d", &stack[*sp]);// *** read(stack[sp])
				break;
			case 11: // sio 3 instruction
				*haltFlag = 1;
				break;
			default:
				printf("This instruction is not recognized\n");
		}
}

// This function simulated the operations of a virtual machine. It takes the list
// of instructions and the output file and runs the instructions.
void runVirtualMachine()
{
	int stack[MAX_DATA_STACK_HEIGHT];
	int sp = MAX_DATA_STACK_HEIGHT;
	int bp = sp - 1;
	int pc = 0;
	int ir = 0;
	int haltFlag = 0;
	int i;
	instruction *currentInstruction;
	instruction instructionsArray[MAX_CODE_LENGTH];
	FILE *ofp = fopen("virtualMachineOutput.txt", "w");
	ofp2 = fopen("codegenOutput.txt", "w");
	ofp3 = fopen("vmExecution.txt", "w");

	fetchInstructions("parserOutput.txt", instructionsArray);
	printInstructions(instructionsArray, ofp);

	fprintf(ofp, "\n\n\n\t\t\tpc\tbp\tsp\tstack\n");
	fprintf(ofp, "Initial values\t\t%d\t%d\t%d\n", pc, bp, sp);
	fprintf(ofp3, "\t\t\tpc\tbp\tsp\tstack\n");
	fprintf(ofp3, "Initial values\t\t%d\t%d\t%d\n", pc, bp, sp);

	// This loop initializes the stack with zeros.
	for (i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
	{
		stack[i] =  0;
	}

	// this while loop takes care of running all the instructions.
	while (haltFlag != 1)
	{
		// get the current instruction and print it
		currentInstruction = &instructionsArray[pc];
		pc++;
		// execute the instruction and print the stack
		executeStep(currentInstruction, stack, &sp, &bp, &pc, &haltFlag, ofp, ofp3);
		fprintf(ofp, "%d %s %d %d\t\t", pc - 1 , OPCODES[currentInstruction->op], currentInstruction->l, currentInstruction->m);
		fprintf(ofp3, "%d %s %d %d\t\t", pc - 1, OPCODES[currentInstruction->op], currentInstruction->l, currentInstruction->m);
		fprintf(ofp, "%d\t%d\t%d\t", pc, bp, sp);
		fprintf(ofp3, "%d\t%d\t%d\t", pc, bp, sp);
		printStack(ofp, stack, bp, sp);
		fprintf(ofp, "\n");
		fprintf(ofp3, "\n");
	}
	fclose(ofp);
	fclose(ofp2);
	fclose(ofp3);
}
