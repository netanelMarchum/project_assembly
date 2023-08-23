#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table_symbol{
    char* name;
    int line;
    char* type;
    struct Table_symbol* next;
}Table_symbol;

/*creates a node to the list.*/
Table_symbol* create_symbol_node(char* name, int line, char* type);

/*adds a node to the end of the list*/
void add_to_table(Table_symbol** symbolList, char* name, int line, char* type);

/*free all the memory in the table*/
void free_table_symbol(Table_symbol* symbolList);

/*checks if the given name is already defined in the list.*/
int defined_symbol(Table_symbol* symbolList, char* name);

/*checks if there is exist this type in the list.*/
int search_type(Table_symbol* symbolList, char* type);

/*checks if the given symbol name is extern.*/
int symbol_is_extern(Table_symbol* symbolList, char* name);

/*returns the line in the memory that the symbol is.*/
int symbol_address(Table_symbol* symbolList, char* name);

/*delete a node and return his name, this function is for "entry" type.*/
int delete_node_of_type(Table_symbol** symbolList, char* typeToDelete,char* label);
