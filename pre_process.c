#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pre_process.h"
#include "table_macro.c"

#define ENDMACRO "endmcro"
#define MACRO "mcro "
#define revahim "\n\t\f\r "
#define NUM_OPERATIONS 49
#define ENDING_FILE_AM ".am"
#define MAX_LENGTH 82

char* operations[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop","auto", "else","long","switch","break","enum","register", "typedef","case", "extern", "return", "union","char", "float", "short", "unsigned","const", "for", "signed", "void","continue", "goto", "sizeof", "volatile","default", "if", "static", "while","do","int", "struct", "_Packed","double"};


struct macro {
    char name[MAX_LENGTH];
    char lines[100][MAX_LENGTH];
    int line_count;
};

struct macro* macro_find(struct macro * macro_table,const int curr_macro_table_size, char * name){
    int i;
    for(i=0;i<curr_macro_table_size;i++) {
        if(strcmp(macro_table[i].name,name) == 0) {
            return &macro_table[i];
        }
    }
    return NULL;
}

enum preprocessor_line_status {
    preprocessor_macro_def,
    preprocessor_macro_enddef,
    preprocessor_macro_call,
    preprocessor_other,
    preprocessor_empty,
    preprocessor_error
};



enum preprocessor_line_status preprocessor_line_check(char *original_line,struct macro **macro_pointer,struct macro * macro_table, int * macro_table_size, int num_line) {
    struct macro* m_search;
    char * line;
    char *pointer;
    char duplicated_line[MAX_LENGTH];
    int i;

    strcpy(duplicated_line,original_line);
    line = &duplicated_line[0];
    while(isspace(*line)) line++;
    if(*line == '\0' || *line == ';')
        return preprocessor_empty;
    pointer = strstr(line,ENDMACRO);
    if(pointer) {
        if(pointer != line) {
	    fprintf(stderr,"Error, in line %d characters were defined before endmacro was defined!\n", num_line);
            return preprocessor_error;
        }
        pointer = strpbrk(pointer,revahim);
        if(pointer) {
            while(isspace(*pointer))pointer++;
            if(*pointer !='\0'){
		fprintf(stderr,"Error, in line %d there are characters after the endmacro definition!\n", num_line);
                return preprocessor_error;
	    }
        }
        return preprocessor_macro_enddef;
    }
    pointer =  strstr(line,MACRO);
    if(pointer) {
        pointer = strpbrk(pointer, revahim);
        if(!pointer) {
            return preprocessor_error;
        }
        while(isspace(*pointer))pointer++;
        if(*pointer == '\0'){
            return preprocessor_error;
	}
        line = pointer;
        pointer = strpbrk(pointer,revahim);
        if(pointer) {
            *pointer = '\0';
            pointer++;
            while(isspace(*pointer))pointer++;
            if(*pointer !='\0'){
		fprintf(stderr,"Error, in line %d there are characters after a macro definition!\n", num_line);
                return preprocessor_error;
	    }
        }

	for (i = 0; i < NUM_OPERATIONS; i++) {
        	if (strcmp(line, operations[i]) == 0) {
            		fprintf(stderr,"Error, in line %d the defined macro name %s is an action name!\n", num_line, line);
                	return preprocessor_error;
        	}
    	}

        m_search = macro_find(macro_table,*macro_table_size,line);
        if(m_search) {
            return preprocessor_error;
        }
        strcpy(macro_table[*macro_table_size].name,line);
        (*macro_pointer) = &macro_table[*macro_table_size];
        (*macro_table_size)++;
        return preprocessor_macro_def;
    }
    pointer = strpbrk(line, revahim);
    if(pointer) {
        *pointer = '\0';
        while(isspace(*pointer))pointer++;
        if(*pointer !='\0') {
            return preprocessor_other;
        }
    }

    m_search = macro_find(macro_table,*macro_table_size,line);
    if(m_search) {
        (*macro_pointer) = m_search;
        return preprocessor_macro_call;
    }
    return preprocessor_other;
}


int preprocessing(FILE *fd, char* name_file){
    FILE* am_file;
    struct macro macro_table[100] = {0};
    int macro_table_size = 0;
    char line[82] = {0};
    int line_counter = 1;
    char *name_file_out;
    struct macro * macro = NULL;
    int i;
    name_file_out = (char*)malloc(strlen(name_file) + 4);
    strcat(strcpy(name_file_out,name_file), ENDING_FILE_AM);

    am_file = fopen(name_file_out,"w");
    while(fgets(line,sizeof(line),fd)) {
        switch(preprocessor_line_check(line,&macro,&macro_table[0],&macro_table_size, line_counter)) {
            case preprocessor_other:
                if(macro == NULL) {
                    fputs(line,am_file);
                }else {
                    strcpy(macro->lines[macro->line_count],line);
                    macro->line_count++;
                }
            break;
                
            case preprocessor_macro_call:
                for(i=0;i<macro->line_count;i++) {
                    fputs(macro->lines[i],am_file);
                }
                macro = NULL;
            case preprocessor_macro_enddef:
                macro = NULL;
            break;
            case preprocessor_error:
                /* we mixed all errors to this state...*/
		remove(name_file_out);
		return 0;
            break;
            default:
            break;

        }
        line_counter++;
    }
    fclose(am_file);
    fclose(fd);
    return 1;
}




