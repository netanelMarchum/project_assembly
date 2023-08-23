#include "secondpass.h"

int The_second_pass(char* b_machine_code,char* str, Table_symbol** symbol_list, int IC, int DC, char* name_file){
	int valid_file=1;
	int count_lines=0;
	int extern_flag=0;
	int temp;
	int length= MAX_LABEL_LENGTH+7;/*length for label,tab and line number.*/
	char* extern_f;
	char* copy_str;/*Copies the original string to manipulate it*/
	char* line;
	char* word;/*pointer*/
	char* current_line;/*for saving the line for new file.*/
	char symbol_name[MAX_LABEL_LENGTH];
	
	extern_f = (char*)calloc(length,sizeof(char));/*for label and line number.*/
	
	current_line=(char*)calloc(length,sizeof(char));
	
	copy_str = (char*)calloc(strlen(str)+1,sizeof(char));
	
	if(!copy_str|| !current_line || !extern_f){
		printf("Error: cant calloc in second pass\n");
		exit(0);
	}
	
	strcpy(copy_str,str);

	line = strtok(copy_str, "\n");
	
	while(line){
		word = line;
		count_lines++;
		
		if(chacksSymbols(word,*symbol_list,count_lines)){/*if the first word is symbol skip it.*/
			word+= skip_word(word);
		}
			
		if(need_to_skip(word)){/*if its not operand word so skip to the next line.*/
			line = strtok(NULL,"\n");
			continue;
		}
		
		if(whichcommand(word) != -1){/*if its opcode skip the op code to the first operand.*/
			word+=skip_word(word);
		}
		
		remove_spaces(word);/*dont need spaces anymore.*/
	
		if(word[0] && its_a_label(word)){
			s_name(symbol_name,word);
			
			if(symbol_is_extern(*symbol_list,symbol_name)){
				
				temp=q_mark_line(b_machine_code);
				extern_f = (char*)realloc(extern_f, strlen(extern_f)+length);
				create_tab_separated_line(current_line,symbol_name,temp);
				strcat(extern_f,current_line);
				strcat(extern_f,"\n");
				
				extern_flag=1;/*turn the ext flag to create extern file.*/
				
				address_update(b_machine_code,1);
				
				line = strtok(NULL,"\n");
				continue;
			}
			
			
			temp = symbol_address(*symbol_list,symbol_name);
			if(!temp){
				printf("Error: in line %d the source operand label is Uninitialized.\n",count_lines);
				valid_file=0;
				line = strtok(NULL,"\n");
				continue;
			}			
			address_update(b_machine_code,temp);
		}
		
		if(!word[0]){/*if this is the end of the line skip to the next line.*/
			line = strtok(NULL,"\n");
			continue;
		}
		word+=skip_word(word)-1;
		
		if(word[0] == ',')		
			word++;
			
		if(word[0] && its_a_label(word)){
			s_name(symbol_name,word);
			
			if(symbol_is_extern(*symbol_list,symbol_name)){
				
				temp=q_mark_line(b_machine_code);
				extern_f = (char*)realloc(extern_f, strlen(extern_f)+length);
				create_tab_separated_line(current_line,symbol_name,temp);
				strcat(extern_f,current_line);
				strcat(extern_f,"\n");
				
				extern_flag=1;
				
				address_update(b_machine_code,1);
				
				line = strtok(NULL,"\n");
				continue;
			}
			
			
			temp = symbol_address(*symbol_list,symbol_name);
			if(!temp){
				printf("Error: in line %d the target operand label is Uninitialized.\n",count_lines);
				valid_file=0;
				line = strtok(NULL,"\n");
				continue;
			}				
			address_update(b_machine_code,temp);
		}
		
		line = strtok(NULL,"\n");
	}
	
	if(!valid_file){/*if there is a errors dont make files.*/
		return 0;
	}
	
	make_ob_file(b_machine_code,IC,DC,name_file);
	
	if(extern_flag)
		make_ext_file(extern_f,name_file);
	
	if(search_type(*symbol_list,"entry"))
		make_ent_file(symbol_list,name_file);
	
	free(copy_str);
	free(current_line);
	free(extern_f);
	free_table_symbol(*symbol_list);
	
	return 1;	
}


int need_to_skip(char* word){
	if(!strncmp(word,".data",5) || !strncmp(word,".string",7) || !strncmp(word,".entry",6) || !strncmp(word,".extern",7)){
		return 1;
	}
	return 0;
}

void address_update(char* b_machine_code,int temp){
	char b_word[WORD_LENGTH+1];
	int i;
	int num=0;
	int R = 2;
	
	if(temp == 1){/*external case.*/
		create_b_word(b_word,1);
	}else{
		num= temp<<2 | R;
		create_b_word(b_word,num);
	}
	
	while(b_machine_code[0] && b_machine_code[0] != '?'){
		b_machine_code++;
	}

	for(i=0; i<12;i++){
		b_machine_code[i] = b_word[i];
	}
}
void create_tab_separated_line(char* current_line, char *word, int number) {
    int len = strlen(word) + 1 + snprintf(NULL, 0, "%d", number) + 1;
    char *line = (char *)malloc(len);
    if (!line) {
        fprintf(stderr, "Memory allocation failed\n");
        return ;
    }
    snprintf(line, len, "%s\t%d\n", word, number);
    strcpy(current_line,line);
}

int q_mark_line(char* b_machine_code){
	int count=BEGINNING_MEMORY;
	
	while(b_machine_code[0] && b_machine_code[0] != '?'){
		if(b_machine_code[0] == '\n')
			count++;
		b_machine_code++;
	}
	return count;
}

void create_ob_file(FILE* fd,char* b_machine_code,int IC,int DC){
	char b_word[12];
	char base64[4];
	
	
	fprintf(fd,"%d %d\n",IC,DC);
	
	while(b_machine_code[0]){
		strncpy(b_word,b_machine_code,12);
		binary_to_base64(b_word,base64);
		fprintf(fd,"%s",base64);
		b_machine_code+=WORD_LENGTH+1;		
	}

}

void binary_to_base64(char* binary_word,char* base64){
    int i;
    unsigned int value = 0;
    char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    
    
    for (i = 0; i < 12; i++) {
        value <<= 1;
        value |= (binary_word[i] - '0');
    }

    base64[0] = base64_table[(value >> 6) & 0x3F];
    base64[1] = base64_table[value & 0x3F];
    base64[2] = '\n';
    base64[3] = '\0';
}

void make_ent_file(Table_symbol** symbol_list,char* name_file){
	char* add_ent;
	char label_name[MAX_LABEL_LENGTH];
	int entry=1;
	int line;
	int len;
	FILE* fd;
	
	len = strlen(name_file)+strlen(".ent");
	
	add_ent = (char*)calloc(len,sizeof(char));
	
	strcat(add_ent,name_file);
	strcat(add_ent,".ent");
	
	if(!(fd = fopen(add_ent,"w+"))){
		printf("Error: can not open .ent file.\n");
		exit(0);
	}
	
	
	while(entry){
		entry =delete_node_of_type(symbol_list,"entry",label_name);
		line = symbol_address(*symbol_list,label_name);
		if(entry == 1)
			fprintf(fd,"%s\t%d\n",label_name,line);		
	}
	
	fclose(fd);

}

void make_ob_file(char* b_machine_code,int IC,int DC,char* name_file){
	char* add_ob;
	FILE* fd;
	int len;
	
	len = strlen(name_file)+strlen(".ob");
	
	add_ob = (char*)calloc(len,sizeof(char));
	
	strcat(add_ob,name_file);
	strcat(add_ob,".ob");

	if(!(fd = fopen(add_ob,"w+"))){
		printf("Error: can not open .ob file.\n");
		exit(0);
	}
	
	create_ob_file(fd,b_machine_code,IC,DC);
	
	fclose(fd);
}

void make_ext_file(char* extern_f,char* name_file){
	char* add_ext;
	FILE* fd;
	int len;
	
	len = strlen(name_file)+strlen(".ext");
	
	add_ext = (char*)calloc(len,sizeof(char));
	
	strcat(add_ext,name_file);
	strcat(add_ext,".ext");

	if(!(fd = fopen(add_ext,"w+"))){
		printf("Error: can not open .ext file.\n");
		exit(0);
	}
	
	fprintf(fd,"%s",extern_f);

	fclose(fd);
}

