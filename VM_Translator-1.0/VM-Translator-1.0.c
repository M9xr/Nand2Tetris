/* Write VM translator for HACK architecture */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_BASE 256	/* base for the pointer in the RAM */
#define MAXCHAR 100	/* Max length of a command in VM code */

int type_check(char *);
int parser(char *, char *);
void itoa(long int, unsigned char *);
void reverse(unsigned char *);

enum {COMMENT, NEW_LINE, INSTRUCTION};

/* 06/05/2024 - changing the structure of the program */
FILE *fp, *fp2;
int SP = STACK_BASE;	/* stack pointer */

int main(int argc, char *argv[]) {

	char *prog = argv[0];	/* program name for errors */
	int name_length;	/* length of the filename */
	char file_to_open[100];

/* Dealing with file */
	if (argc != 2) {
		fprintf(stderr, "%s: Creates assembly version of VM code\nUsage: prompt>VM_Translator Program.vm\n",prog+2);
		exit(1);
	}
	name_length = strlen(argv[1]);
	/* check if the name of the file is valid */
	if (!(isupper(*(argv[1])))) {
		fprintf(stderr, "%s Error: First character of the file name must be an uppercase latter\n", prog+2);
		exit(2);
	}
	if (strcmp(".vm", argv[1]+name_length-3)) {
		fprintf(stderr, "%s Error: File %s has no .vm extension\n", prog+2, argv[1]);
		exit(3);
	}
	strncpy(file_to_open, argv[1], name_length-3);
	file_to_open[name_length-3] = '\0';
	printf ("The file so far is %s\n", file_to_open);
	strcat(file_to_open, ".asm");
	file_to_open[name_length+1] = '\0';

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "%s can't open %s\n",
				prog+2, argv[1]);
		exit(4);
	}
	if ((fp2 = fopen(file_to_open, "w")) == NULL) {
		fprintf(stderr, "%s can't create %s\n",
				prog+2, file_to_open);
		exit(4);
	}
	printf ("The created file is %s\n", file_to_open);

	
	char line[MAXCHAR], *lineptr, line_asm[MAXCHAR];
	lineptr = line;
	int value_type;

/* Stack initialization */
	char ss1[] = "@256\nD=A\n@SP\nM=D\n";
	if (fputs(ss1, fp2) == EOF) {
		printf("fputs1 error\n");
		exit(5);
	}


	while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
		value_type = type_check(line);
		if (value_type == INSTRUCTION) {
			parser(line, line_asm);
			printf("INSTRUCTION: %s", line);
			// if (fpust(line_asm, fp2) == EOF } ...
		}
		else if (value_type == COMMENT) {
			printf("COMMENT: %s", line);
		}
		else {
			printf("A NEW LINE: %s", line);
		}

	}

/* Stack ending */
	printf("EXIT TEST1\n");
	if (fputs("(END)\n@END\n0;JMP", fp2) == EOF) {
		printf("fputs 5 error\n");
		exit(5);
	}
	printf("EXIT TEST2\n");
	exit(0);
}

/* type_check: return type of the instruction */
int type_check(char *s) {

	while (isspace(*s))
		++s;
	if (isalpha(*s))
		return INSTRUCTION;
	else if (*s == '/')
		return COMMENT;
	else
		return NEW_LINE;
}


enum {C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL};

/* ints for labels for comaparasion commands (eq, gt, lt) */
int LABEL_NO = 0;
int EXIT_NO = 0;

int parser(char *line_vm, char *line_asm) {

		char label_number[100], label_number2[100], exit_number[100], stack_number[100];
		char *string_ptr = line_vm;
		printf("THE COPIED STRING: %s\n", string_ptr);
		int i = 0;
		
		
		if (strstr(line_vm, "push")) {
			/* obtaining the number */
			while (!isdigit(*string_ptr))
				++string_ptr;
			stack_number[i++] = *string_ptr++;
			while (isdigit(*string_ptr))
				stack_number[i++] = *string_ptr++;
			stack_number[i] = '\0';
			printf("**Push number:%s\n", stack_number);
			/* get the value */
			if (fputs("@", fp2) == EOF)
				exit(5);
			if (fputs(stack_number, fp2) == EOF)
				exit(5);


			if (strstr(line_vm, "constant")) {
				if (fputs("\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) { //Checked
					printf("Och jo 0\n");
					exit(5);
				}
			}

			/* TO DO: to make the code prettier, build an additional parser for parsing list of arguments into a single string. something like printf from Chapter 7 K&R. */
			 
			else if (strstr(line_vm, "local")) {
				if (fputs("\nD=A\n@LCL\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("X1 Local\n");
			}
			else if (strstr(line_vm, "this")) {
				if (fputs("\nD=A\n@THIS\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("X2 this\n");
			}
			else if (strstr(line_vm, "that")) {
				if (fputs("\nD=A\n@THAT\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("X3 that\n");
			}
			else if (strstr(line_vm, "temp")) {
				if (fputs("\nD=A\n@5\nA=A+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("X4 temp\n");
			}
			else if (strstr(line_vm, "argument")) {
				if (fputs("\nD=A\n@ARG\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("X5 argument\n");
			}

			/* pointers */
			else if (strstr(line_vm, "pointer 0")) {
				if (fputs("\n@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
			}
			else if (strstr(line_vm, "pointer 1")) {
				if (fputs("\n@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
			}
			else if (strstr(line_vm, "static")) {
				if (fputs("\n@static", fp2) == EOF)
					exit(5);
				if (fputs(stack_number, fp2) == EOF)
					exit(5);
				if (fputs("\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
			}



		}
		else if (strstr(line_vm, "pop")) {

			/* obtaining the number */
			while (!isdigit(*string_ptr)) {
				printf("%c", *string_ptr);
				++string_ptr;
			}
			stack_number[i++] = *string_ptr;
			++string_ptr;
			while (isdigit(*string_ptr)) {
				stack_number[i++] = *string_ptr++;
			}
			stack_number[i] = '\0';
			/* get the value */
			if (fputs("@", fp2) == EOF)
				exit(5);
			if (fputs(stack_number, fp2) == EOF)
			exit(5);

			if (strstr(line_vm, "argument")) {
			/* Get the index of the virutal segment, then get the base address and the base address to the index. Finally, get the value into the address+index address */
				if (fputs("\nD=A\n@R13\nM=D\n@ARG\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("Y1 argument\n");
			}
			else if (strstr(line_vm, "local")) {
				if (fputs("\nD=A\n@R13\nM=D\n@LCL\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("Y1 local\n");
			}
			else if (strstr(line_vm, "this"))  {
				if (fputs("\nD=A\n@R13\nM=D\n@THIS\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("Y1 this\n");
			}
			else if (strstr(line_vm, "that")) {
				if (fputs("\nD=A\n@R13\nM=D\n@THAT\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("Y1 that\n");
			}
			else if (strstr(line_vm, "temp")) {
				if (fputs("\nD=A\n@R13\nM=D\n@5\nD=A\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
			}
			else if (strstr(line_vm, "pointer 0")) {
				if (fputs("\n@SP\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n", fp2) == EOF)
					exit(5);
			}
			else if (strstr(line_vm, "pointer 1")) {
				if (fputs("\n@SP\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n", fp2) == EOF)
					exit(5);
			}
			else if (strstr(line_vm, "static")) {
				if (fputs("\n@SP\nM=M-1\nA=M\nD=M\n@static", fp2) == EOF)
					exit(5);
				if (fputs(stack_number, fp2) == EOF)
					exit(5);
				if (fputs("\nM=D\n", fp2) == EOF)
					exit(5);
			}

						

		}



	else if (strstr(line_vm, "add")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M+D\n@SP\nM=M+1\n", fp2) == EOF)   
			exit(5);
	}
	else if (strstr(line_vm, "sub")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M-D\n@SP\nM=M+1\n", fp2) == EOF) 
			exit(5);
	}
	else if (strstr(line_vm, "neg")) {
		if (fputs("@SP\nA=M-1\nM=-M\n", fp2) == EOF)  
			exit(5);
	}
	else if (strstr(line_vm, "or")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M|D\n@SP\nM=M+1\n", fp2) == EOF) 
			exit(5);
	}
	else if (strstr(line_vm, "and")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D&M\n@SP\nM=M+1\n", fp2) == EOF)
			exit(5);
	}
	else if (strstr(line_vm, "not")) {
		if (fputs("@SP\nA=M-1\nM=!M\n", fp2) == EOF)
			exit(5);
	}



	else if (strstr(line_vm, "eq") || strstr(line_vm, "lt") || strstr(line_vm, "gt")) {
		printf("eq eq\n");
		++LABEL_NO;
		itoa(LABEL_NO, label_number);
		++LABEL_NO;
		itoa(LABEL_NO, label_number2);
		++EXIT_NO;
		itoa(EXIT_NO, exit_number);
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@label", fp2) == EOF)
			exit(5);
		if (fputs(label_number, fp2) == EOF)
			exit(5);

		if (strstr(line_vm, "eq")) {
			if (fputs("\nD;JEQ\n", fp2) == EOF)
				exit(5);
		}
		else if (strstr(line_vm, "lt")) {
			if (fputs("\nD;JLT\n", fp2) == EOF)
				exit(5);
		}
		else {
			if (fputs("\nD;JGT\n", fp2) == EOF)
				exit(5);
		}
		
		
		if (fputs("@label", fp2) == EOF)
			exit(5);

		if (fputs(label_number2, fp2) == EOF)
			exit(5);
		if (fputs("\n0;JMP\n(label", fp2) == EOF)
			exit(5);
		if (fputs(label_number, fp2) == EOF)
			exit(5);
		if (fputs(")\n@SP\nA=M\nM=-1\n@exit", fp2) == EOF)
			exit(5);
		if (fputs(exit_number, fp2) == EOF)
			exit(5);
		if (fputs("\n0;JMP\n(label", fp2) == EOF)
			exit(5);
		if (fputs(label_number2, fp2) == EOF)
			exit(5);
		if (fputs(")\n@SP\nA=M\nM=0\n@exit", fp2) == EOF)
			exit(5);
		if (fputs(exit_number, fp2) == EOF)
			exit(5);
		if (fputs("\n0;JMP\n(exit", fp2) == EOF)
			exit(5);
		if (fputs(exit_number, fp2) == EOF)
			exit(5);
		if (fputs(")\n@SP\nM=M+1\n", fp2) == EOF)
			exit(5);
	}

	return 0;
}


/* itoa: convert n to characters in s */
void itoa(long int n, unsigned char s[]) {

	int i, sign;

	if ((sign = n) < 0)
		n=-n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

/* reverse: reverse string s in place */
void reverse(unsigned char s[]) {

	int c, i, j;
	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}


}
