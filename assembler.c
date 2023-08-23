#include "assembler.h"
#include "pre_process.h"

int main(int argc, char* argv[]){
    input_files(argc,argv);
    return 1;
}

void input_files(int argc, char* argv[]){
    FILE *file;
    int i;
    char* name_of_file;
    int pre_process_succeeded=0;
    char* str_of_file_am;
    /*Table_symbol* symbol_list;*/

    if(argc < 1) {
        fprintf(stderr,"Error, no file name inserted!\n");
        exit(0);
    }

    for(i=1; i<argc ;i++){
        name_of_file = (char*)malloc(strlen(argv[i]) + 4); /*4 For .as*/

        if (!name_of_file){
            continue; /* continue to read the next file*/
        }

        strcpy(name_of_file, argv[i]);
        strcat(name_of_file, ENDING_FILE_AS);

        file = fopen(name_of_file, "r");
        if(!file){
            fprintf(stderr,"Error, file %s not opened!\n", argv[i]);
	        continue;
        }

        free(name_of_file);
        pre_process_succeeded = preprocessing(file, argv[i]);
        
	if (pre_process_succeeded){
            str_of_file_am = open_am_file(argv[i]);
	
           if (str_of_file_am != NULL){
                if (The_first_pass(str_of_file_am, argv[i])){
                    free(str_of_file_am);
                }
           }


       }
    }
}

char* open_am_file(char* name_of_file) {
    FILE* file;
    char* contents_file;
    char* name_file_out;
    long file_size;

    name_file_out  = (char*)malloc(strlen(name_of_file) + 4); /*4 For .am*/

    if (name_file_out == NULL) {
        fprintf(stderr, "Error, Memory allocation of saving file name with .am extension!\n");
        return NULL;
    }

    strcpy(name_file_out, name_of_file);
    strcat(name_file_out, ENDING_FILE_AM);


    file = fopen(name_file_out, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: The file %s could not be opened!\n", name_file_out);
        free(name_file_out);
        return NULL;
    }

    fseek(file, 0, SEEK_END); /*Moves the pointer to the last of the file*/
    file_size = ftell(file); /*Calculates the size of the file*/
    fseek(file, 0, SEEK_SET); /*Moves the pointer to the beginning of the file*/


    contents_file = (char*)malloc(file_size + 1);
    if (contents_file == NULL) {
        fprintf(stderr, "Error, memory allocation of save file contents failed!\n");
        fclose(file);
        return NULL;
    }

    fread(contents_file, file_size, 1, file);
    contents_file[file_size] = '\0';

    fclose(file);
    free(name_file_out);

    return contents_file;
}
