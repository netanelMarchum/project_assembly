assembler: assembler.o pre_process.o firstpass.o secondpass.o table_symbol.o table_macro.o
	gcc -g -Wall -ansi -pedantic assembler.o pre_process.o firstpass.o secondpass.o table_symbol.o table_macro.o -o assembler

assembler.o: assembler.c assembler.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

pre_process.o: pre_process.c pre_process.h
	gcc -c -Wall -ansi -pedantic pre_process.c -o pre_process.o

firstpass.o: firstpass.h table_symbol.h
	gcc -c -Wall -ansi -pedantic firstpass.c -o firstpass.o

secondpass.o: secondpass.h firstpass.h table_symbol.h
	gcc -c -Wall -ansi -pedantic secondpass.c -o secondpass.o

table_symbol.o: table_symbol.h
	gcc -c -Wall -ansi -pedantic table_symbol.c -o table_symbol.o

