#include "table_symbol.h"

Table_symbol* create_symbol_node(char* name, int line, char* type){
    Table_symbol* symbol_node = (Table_symbol*)malloc(sizeof(Table_symbol));

	if(symbol_node){
            symbol_node->name = (char*)malloc(strlen(name) + 1);
            symbol_node->type = (char*)malloc(strlen(type) + 1);
	}
        if (symbol_node->name && symbol_node->type) {
            strcpy(symbol_node->name, name);
            strcpy(symbol_node->type, type);
            symbol_node->line = line;
            symbol_node->next = NULL;
        } else {
            free(symbol_node->name); 
            free(symbol_node->type); 
            free(symbol_node);
            symbol_node = NULL;
        }
    return symbol_node;
}


void add_to_table(Table_symbol** symbolList, char* name, int line, char* type) {
    Table_symbol* newNode = create_symbol_node(name, line, type);
    if (newNode) {
        newNode->next = NULL;

        if (!*symbolList) {
            *symbolList = newNode;
        }
        else {
            Table_symbol* current = *symbolList;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }
}

void free_table_symbol(Table_symbol* symbolList) {
    Table_symbol* current = symbolList;
    Table_symbol* next;

    while (current != NULL) {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }

}

int defined_symbol(Table_symbol* symbolList, char* name){
    Table_symbol* ptr;

    if(!symbolList){
        return 0;
    }
    ptr = symbolList;
    while (ptr){
        if(!strcmp(ptr->name, name)){
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int search_type(Table_symbol* symbolList, char* type){
    Table_symbol* ptr;

    if(symbolList == NULL){
        return 0;
    }
    ptr = symbolList;
    while (ptr !=NULL){
        if(strcmp(ptr->type, type) == 0){
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int symbol_is_extern(Table_symbol* symbolList, char* name){
    Table_symbol* ptr;

    if(symbolList == NULL){
        return 0;
    }
    ptr = symbolList;
    while (ptr !=NULL){
        if(!strcmp(ptr->name, name)){
            return strcmp(ptr->type,"external") ? 0 : 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int symbol_address(Table_symbol* symbolList, char* name){
    Table_symbol* ptr;

    if(symbolList == NULL){
        return 0;
    }
    ptr = symbolList;
    while (ptr){
        if(!strcmp(ptr->name, name) && strcmp(ptr->type,"entry")){
            return ptr->line;
        }
        ptr = ptr->next;
    }
    return 0;
}

int delete_node_of_type(Table_symbol** symbolList, char* typeToDelete,char* label) {
    Table_symbol* current = *symbolList;
    Table_symbol* prev = NULL;


    while (current != NULL) {
        if (strcmp(current->type, typeToDelete) == 0) {
            if (prev == NULL) {
                *symbolList = current->next;
            } else {
                prev->next = current->next;
            }
            strcpy(label,current->name);
            free(current->type);
            free(current);
            return 1;
        } else {
            prev = current;
            current = current->next;
        }
    }
    return 0;
}
