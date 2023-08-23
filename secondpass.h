#include "firstpass.h"

/*this method check if need to skip the first word.*/
int need_to_skip(char* word);

/*this method set the temp binary number to the first unknow word in b mchine code.*/
void address_update(char* b_machine_code,int temp);

/*this method create a line for the extern file.*/
void create_tab_separated_line(char* current_line, char *word, int number);

/*this method return where is the first word in the b machine code which has not been initialized yet*/
int q_mark_line(char* b_machine_code);

/*this method create the and print the ".ob" string to the file*/
void create_ob_file(FILE* fd,char* b_machine_code,int IC,int DC);

/*this method gets first word as a 12-character binary code, transferring it to base 64 and saves it in the second string*/
void binary_to_base64(char* binary_word,char* base64);

/*this method makes ent file.*/
void make_ent_file(Table_symbol** symbol_list,char* name_file);

/*this method makes ob file.*/
void make_ob_file(char* b_machine_code,int IC,int DC,char* name_file);

/*this method makes ext file.*/
void make_ext_file(char* extern_f,char* name_file);
