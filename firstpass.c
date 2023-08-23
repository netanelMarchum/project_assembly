#include "firstpass.h"

char *opCode[OPER] = {"mov","cmp","add","sub","not","clr","lea","inc",
"dec","jmp","bne","red","prn","jsr","rts","stop"};

char *registers[REGISTERS_AMOUNT] = {"@r0","@r1","@r2","@r3","@r4","@r5","@r6","@r7"};

enum {mov=0,cmp,add,sub,Not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop};

int The_first_pass(char* str, char* name_file){
	int DC =0;
	int IC =0;
	int its_a_symbol=0;/*flag if there is a symbol.*/
	int valid_file=1;/*Indication if the first pass was successful*/
	int count_lines=0;
	int temp;
	char* word;/*pointer*/
	char* data_b_code;
	char* inst_b_code;/*Instruction binary code.*/
	char* copy_str;/*Copies the original string to manipulate it*/
	char* line;
	char symbol_name[MAX_LABEL_LENGTH];
	Table_symbol** symbol_list;

	copy_str = (char*)calloc(strlen(str)+1,sizeof(char));
	
	symbol_list = (Table_symbol**)calloc(sizeof(Table_symbol**),sizeof(char));
	
	data_b_code = (char*)calloc(WORD_LENGTH+1,sizeof(char));
	
	inst_b_code = (char*)calloc(WORD_LENGTH+1,sizeof(char));
	
	if(!copy_str|| !symbol_list || !data_b_code || !inst_b_code){
		printf("Error: cant calloc in first pass\n");
		exit(0);
	}

	strcpy(copy_str,str);
	
	line = strtok(copy_str, "\n");
	
	while(line){
		word = line;
		count_lines++;
		
		its_a_symbol = chacksSymbols(word,*symbol_list,count_lines);/*checks if the first word is symbol*/
		
		if(its_a_symbol == -1){/*if the first word is illegal symbol.*/
			valid_file=0;
			line = strtok(NULL,"\n");
			continue;
		}
		
		if(its_a_symbol){
			s_name(symbol_name,word);/*saving the symbol name*/
			word+= skip_word(word);/*skipping to the next word*/
		}

		if(its_a_data_storage(word)){/*5-7*/
			
			if(its_a_symbol)
				add_to_table(symbol_list,symbol_name,DC+IC+BEGINNING_MEMORY,"data");
				
			temp= how_many_data(word);
			data_b_code = (char*)realloc(data_b_code,strlen(data_b_code)+temp*(WORD_LENGTH+1));	
			temp= add_b_data_code(data_b_code,word,count_lines);
			if(temp == ILLEGAL)
				valid_file=0;
			
			DC+= temp % ILLEGAL;
			line = strtok(NULL,"\n");
		continue;
		}
		
		remove_spaces(word);/*After checks that it is not data the spaces are no longer relevant*/
		
					/*8-10*/
			if(!(strncmp(word,".extern",7))){/*extern case.*/
			
				word+=strlen(".extern");
				while(word[0]){
					if(its_a_label(word)){
						s_name(symbol_name,word);
						add_to_table(symbol_list,symbol_name,0,"external");
					}
				word +=skip_word(word);
				}
				line = strtok(NULL,"\n");
				continue;
			}
			
			if(!(strncmp(word,".entry",6))){/*entry case.*/
				word+=strlen(".entry");
				if(its_a_label(word)){
					s_name(symbol_name,word);
					add_to_table(symbol_list,symbol_name,0,"entry");
				}
				word +=skip_word(word);
				line = strtok(NULL,"\n");
				continue;
			}

		if(its_a_symbol)/*11*/
			add_to_table(symbol_list,symbol_name,DC+IC+BEGINNING_MEMORY,"code");

		inst_b_code = (char*)realloc(inst_b_code,strlen(inst_b_code)+3*(WORD_LENGTH+1));/*3 is the max words to add per interaction.*/
		temp = add_b_inst_code(inst_b_code,word,count_lines);/*this is "L"*/
		if(temp == ILLEGAL)
			valid_file=0;
		
		IC+= temp % ILLEGAL;
	
	line = strtok(NULL,"\n");
	}
	
	if(DC+IC > 924)/*if the memory overflows.*/
		valid_file=0;
	
	
	if(!valid_file)/*16*/
		return 0;
	
	inst_b_code = (char*)realloc(inst_b_code,strlen(inst_b_code)+strlen(data_b_code));
	
	strcat(inst_b_code,data_b_code);
	
	free(copy_str);/*free the memory we didnt use in sec pass.*/
	
	free(data_b_code);
	
	The_second_pass(inst_b_code,str,symbol_list,IC,DC, name_file);
	
	return 1;
}

int chacksSymbols(char* firstword,Table_symbol* symbol_list, int count_lines){
char ch = firstword[0];
int error_num = 0;
int i =0;

	if(its_a_saved_word(firstword))/*if the label its a saved word.*/
		error_num = 1;
		
	if(defined_symbol(symbol_list,firstword))/*if the label is already defined.*/
		error_num = 2;

	if ((ch<'a' || ch>'z') && (ch<'A' || ch>'Z')) /*checks the first char.*/
		error_num = 3;
		
	ch=firstword[++i];

	while(firstword[i] && ch != ' ' && ch != '\n' && ch != ':'){
		if((ch<'a' || ch>'z') && (ch<'A' || ch>'Z') && (ch<'0' || ch>'9'))
			error_num = 4;
	
		ch = firstword[++i];
	}
	
	if(ch == ':'){
		switch(error_num){
			case 0:/*in case no errors*/
				return 1;
			case 1:
				printf("Error: in line %d the symbol is a reserved word\n",count_lines);
				return -1;
			case 2:
				printf("Error: in line %d the symbol is already defined\n",count_lines);
				return -1;
			case 3:
				printf("Error: in line %d the beginning of the symbol is invalid\n",count_lines);
				return -1;
			case 4:
				printf("Error: in line %d there is an invalid char in the symbol\n",count_lines);
				return -1;
		}
	}
	return 0;
}

int its_a_saved_word(char* p){
	int i,len;
	
	for(i=0; i < OPER; i++){
		len = strlen(opCode[i]);
		
		if(!strncmp(p,opCode[i],len)){
			return 1;
		}
	}

return 0;
}

int its_a_data_storage(char *word){
if(!(strncmp(word,".string",7)) || !(strncmp(word,".data",5)))
	return 1;
return 0;
}

int how_many_data(char* word){
int count=0;

	if(!strncmp(word,".string",7)){
		word+=strlen(".string");
		while(word[0] && word[0] != '"'){/*skipping until the data.*/
			word++;
		}
		word++;/*skip the '"'*/
		
		while(word[0] && word[0] != '"'){
			word++;
			count++;
		}
		return ++count;/*for the end of string.*/
	}/*end of string case.*/

	while(word[0]){/*count how many data has, doesnt check legality.*/
		word++;
		if(word[0] == ',')
			count++;
	}
	return ++count;/*for n numbers need n-1 comma.*/
}

void s_name(char* name,char* word){
	int i=0;
	
	while(word[i] && word[i] != ':' && word[i] != ','){
		name[i] = word[i];
		i++;
	}
	name[i] = '\0';
}

int skip_word(char* word){
	int count=0;
	char ch=word[count];
	
	while(ch && ch != ' ' && ch != '\t' && ch != ','){
		count++;
		ch=word[count];
	}
	count++;
	return count;
}

int add_b_data_code(char* b_code, char* word,int count_lines){
int count_b_word=0;
char b_word[13];/*plus 1 for the \n in the end.*/

	if(!strncmp(word,".string",7)){
		word+=strlen(".string");
		while(word && word[0] != '"'){
			if(word[0] != ' ' && word[0] != '\t'){
				printf("Error: in line %d extra char between .string to \"data\"\n",count_lines);
				return ILLEGAL;
			}
			word++;
		}
		word++;/*skipping the '"'*/
		while(word && word[0] != '"'){
			
			create_b_word(b_word,(int)word[0]);
			
			strcat(b_code,b_word);
			count_b_word++;
			word++;
		}
		
		create_b_word(b_word,0);
		
		b_word[12]= '\n';
		strcat(b_code,b_word);
		count_b_word++;
		word++;
		/*fuction to check if there are extra char.*/
	}/*end of ".string" case.*/
	
	if(!strncmp(word,".data",5)){
		word+=strlen(".data");
		remove_spaces(word);
		
		if(!word[0]){
			printf("Error: in line %d there is empty data\n",count_lines);
			return ILLEGAL;
		}
		
		while(word[0]){
			if(its_not_num(word)){
				printf("Error: in line %d is an invalid number after \".data\"\n",count_lines);
				return ILLEGAL;
			}
			
			create_b_word(b_word,atoi(word));
			
				strcat(b_code,b_word);
				count_b_word++;
				word+=skip_digits(word);
				if(word && word[0] == ',')
					word++;
		}
	}
	return count_b_word;
}

void create_b_word(char* b_word ,int value){
int mask,bit;
int i=0;

	if(value >= 0){
		for(i = 11; i >= 0; i--){
			bit = (value >> i) & 1;
			b_word[11 - i] = '0' + bit;
		}
	}else{
		mask = 1 << 11; 
		for(i = 0; i < 12; i++){
			bit = (value & mask) ? 1 : 0;
			b_word[i] = '0' + bit; 
			mask >>= 1;
		}
	}
	b_word[12] = '\n';

}

int its_not_num(char *p){
	int i=0;
	
	if(p[0] == '-' || p[0] == '+')
		i++;
	
	for(;p[i] && p[i] != ',' ;i++){
		if(!isdigit(p[i])) 
			return 1;
	}		
			
	if(i == 0 || (i == 1 && (p[0] == '-' || p[0] == '+'))){
		return 1;
		}
	
	return 0;

}

int skip_digits(char *p){
	int count=0;
	
	if(p[0] == '-'|| p[0] == '+')
		count++;
	
	while(isdigit(p[count]))
		count++;
	
	return count;
}


/*this faction removing all the spaces from the line.*/
void remove_spaces(char *str) {
	int i;
	int count = 0;
	
	for (i = 0; str[i]; i++){/*as long as there are characters in the string*/
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
			str[count] = str[i];
			count++;
		}
	}
	
	str[count] = '\0';/*end of the string*/
}

int add_b_inst_code(char* b_code, char* word, int count_lines){
char b_word[13];/*plus 1 for the \n in the end.*/
int op_number, temp;
int s_num, s_register_num, t_num, t_register_num;
int target_flag=0;
int source_flag=0;
int count_b_word=0;
s_register_num=t_register_num=0;

	op_number=whichcommand(word);

	remove_spaces(word);
	
	switch(op_number){
		case mov:
		case cmp:
		case add:
		case sub:
		case lea:
			
			word+=strlen(opCode[op_number]);/*skiping the op code.*/
			
			if(!word[0]){
				printf("Error: missing arguments in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			if(!its_not_num(word)){/*if the first oper is num*/
				if(op_number == lea){
				printf("error: in line %d ilegal destination operand.\n",count_lines);
				return ILLEGAL;
				}
				s_num = atoi(word);
				
				word+=skip_digits(word);
				
				source_flag=1;	
			}
			
			if(!source_flag && which_reg(word) != -1){/*if the first oper is reg.*/
				if(op_number == lea){
				printf("Error: in line %d ilegal destination operand.\n",count_lines);
				return ILLEGAL;
				}
				
				s_register_num = which_reg(word);
				
				word+=3;/*skiping the length of register.*/
			
				source_flag=5;
			}
			
			if(!source_flag && its_a_label(word)){/*if the first oper is label.*/
				word+=skip_label(word);
				
				source_flag=3;
			}
			
			if(!word){
				printf("Error: missing arguments in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			if(word[0] != ','){
				printf("Error: missing comma in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			word++;/*skipping comma*/
			
			if(op_number == cmp && !its_not_num(word)){
				t_num = atoi(word);
				
				word+=skip_digits(word);
				
				target_flag=1;
			}
			
			
			if(!target_flag && which_reg(word) != -1){
				
				t_register_num = which_reg(word);
				
				word+=3;/*skiping the length of register.*/
			
				target_flag=5;
			}
			
			
			if(!target_flag && its_a_label(word)){
				word+=skip_label(word);
				target_flag=3;
			}
			
			if(word[0]){
				printf("Error: extra charchters in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			if(!target_flag || !source_flag){
				printf("Error: missing operand in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			temp= target_flag<<2 | op_number<<5 | source_flag<<9;
			create_b_word(b_word,temp);
			
			strcat(b_code,b_word);
			count_b_word++;
			
			if(source_flag == 1){
				create_b_word(b_word,s_num);
				
				strcat(b_code,b_word);
				count_b_word++;
			}
			
			if(source_flag == 3){/*in case its label, we enter the adress in the sec pass.*/
				strcat(b_code,"????????????\n");
				count_b_word++;
			}
			
			if(source_flag == 5 || target_flag == 5){
				
				temp = t_register_num<<2 | s_register_num<<7;
				create_b_word(b_word,temp);
				
				strcat(b_code,b_word);
				count_b_word++;
			}
			
			if(target_flag == 1){
				create_b_word(b_word,t_num);
				
				strcat(b_code,b_word);
				count_b_word++;
			}
			
			if(target_flag == 3){/*in case its label*/
				strcat(b_code,"????????????\n");
				count_b_word++;
			}
			
			return count_b_word;
			
			break;
		case Not:
		case clr:
		case inc:
		case dec:
		case jmp:
		case bne:
		case red:
		case prn:
		case jsr:
			
			word+=strlen(opCode[op_number]);/*skiping the op code.*/
			
			if(!word[0]){
				printf("Error: missing arguments in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			if(op_number == prn && !its_not_num(word)){
				t_num = atoi(word);
				
				word+=skip_digits(word);
				
				target_flag=1;
			}
			
			
			if(!target_flag && which_reg(word) != -1){
				
				t_register_num = which_reg(word);
				
				word+=3;/*skiping the length of register.*/
			
				target_flag=5;
			}
			
			
			if(!target_flag && its_a_label(word)){
				word+=skip_label(word);
				target_flag=3;
			}
			
			if(word[0]){
				printf("Error: extra charchters in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			if(!target_flag){
				printf("Error: missing operand in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			temp= target_flag<<2 | op_number<<5;
			create_b_word(b_word,temp);
			
			strcat(b_code,b_word);
			count_b_word++;
			
			if(target_flag == 5){
				
				temp = t_register_num<<2;
				create_b_word(b_word,temp);
				strcat(b_code,b_word);
				count_b_word++;
			}
			
			if(target_flag == 1){
				create_b_word(b_word,t_num);
				
				strcat(b_code,b_word);
				count_b_word++;
			}
			
			if(target_flag == 3){/*in case its label*/
				strcat(b_code,"????????????\n");
				count_b_word++;
			}
			
			return count_b_word;
			
			break;
		case rts:
		case stop:
			
			word+=strlen(opCode[op_number]);/*skiping the op code.*/
			
			if(word[0]){
				printf("Error: extra charchters in line %d\n",count_lines);
				return ILLEGAL;
			}
			
			temp= op_number<<5;
			create_b_word(b_word,temp);
			
			strcat(b_code,b_word);
			count_b_word++;
			
			return count_b_word;
			
			break;
		default:
			printf("Error: in line %d Unknown opcode.\n",count_lines);
			return ILLEGAL;
			break;
	}/*end of switch*/
}

int skip_label(char* word){
	int i=0;
	while(word[i] && word[i] != ','){
		i++;
	}
	return i;
}

int whichcommand (char* word){
	int i,len,space_count=0;
	
	while(word && (word[space_count] == ' '|| word[space_count] == '\t'))
		space_count++;/*to ignoring space from before the command*/
	
	for(i=0; i< OPER; i++){
		len = strlen(opCode[i]);
		
		if(strncmp(word+space_count,opCode[i],len)==0){
			return i;
		}
	}

	return -1;
}

int which_reg(char* word){
	int i;
	int len=3;/*the length of the string "@rn"  0<=n<=7*/

	for(i=0;i< REGISTERS_AMOUNT; i++){
		if(!strncmp(word,registers[i],len))
			return i;
	}
	return -1;
}

int its_a_label(char* word){
char ch = word[0];
int i =0;
	
	if(its_a_saved_word(word))
		return 0;
	
	if ((ch<'a' || ch>'z') && (ch<'A' || ch>'Z')) 
		return 0;
		
	ch=word[++i];

	while(word[i] && ch != ' ' && ch != '\n' && ch != ','){
		if((ch<'a' || ch>'z') && (ch<'A' || ch>'Z') && (ch<'0' || ch>'9'))
			return 0;
		ch = word[++i];
	}
	return 1;	
}
