#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ENDING_FILE_AM ".am"
#define ENDING_FILE_AS ".as"


/*
    The function receives a number of files from the terminal as arguments and starts performing the "preprocessor" step.
*/
void input_files(int argc, char* argv[]);

/*
    The function get file checks if the file exists with an AM extension and returns the contents of the file
*/
char* open_am_file(char* name_of_file);


int The_first_pass(char* str, char* namefile);
