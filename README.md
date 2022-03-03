Have a folder called Compiler on your desktop with hw4compiler.c, 
lex.c, lex.h, parser.c, parser.h, codegen.c, codegen.h, vm.c and vm.h
and the test input files.

Open your terminal and enter the following:

cd Desktop
cd compiler
gcc hw4compiler.c lex.c parser.c codegen.c vm.c
./a.out <input file name here>.txt <additional flags here>

The main output will appear in the terminal. The output of each 
individual program will appear in the files
- lexOutput.txt
- parserOutput.txt
- virtualMachineOutput.txt
