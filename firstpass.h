#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table_symbol.h"

#define MAX_LINE_LENGTH 81
#define OPER 16
#define MAX_LABEL_LENGTH 31
#define WORD_LENGTH 12
#define REGISTERS_AMOUNT 8
#define BEGINNING_MEMORY 100
#define ILLEGAL 10000

/*the second pass of the assmbler.*/
int The_first_pass(char* str, char* name_file);

/*the second pass of the assmbler.*/
int The_second_pass(char* b_machine_code,char* str, Table_symbol** symbol_list, int IC, int DC, char* name_file);

/*checks if there is definition of symbol in the first word.*/
int chacksSymbols(char* firstword,Table_symbol* symbol_list,int count_lines);

/*this method checks if the given param is a saved word.*/
int its_a_saved_word(char* p);

/*this method checks if the given param start with ".data" or ".string"*/
int its_a_data_storage(char *word);

/*this method calc how many word need to enter to the machine code.(for realloc)*/
int how_many_data(char* word);

/*this method save the sec word in the first one.*/
void s_name(char* name,char* word);

/*this methods return how many char need to skip the first word.*/
int skip_word(char* word);

/*this method add data binary words to the binary code.*/
int add_b_data_code(char* b_code, char* word, int count_lines);

/*this method create binary word from the given value and set the result in b_word(binary word)
*if the value is negative, it returns the Two's complement.*/
void create_b_word(char* b_word ,int value);

/*this method checks if the first word is not a number.*/
int its_not_num(char *p);

/*this method return how many char to skip the first number.*/
int skip_digits(char *p);

/*this method remove all the spaces from a given string.*/
void remove_spaces(char *str);

/*this method add instraction binary words to the binary code.*/
int add_b_inst_code(char* b_code, char* word, int count_lines);

/*this method return which op code is the first word, return -1 in case the first word isnt.*/
int whichcommand (char* word);

/*this method return which register is the first word.*/
int which_reg(char* word);

/*this method return if the first word is a legal label.*/
int its_a_label(char* word);

/*this method return how many char need to skip the first word label.*/
int skip_label(char* word);





