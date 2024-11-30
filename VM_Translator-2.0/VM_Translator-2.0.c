/* Write VM translator for HACK architecture */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#define STACK_BASE 256	/* base for the pointer in the RAM */
#define MAXCHAR 100	/* Max length of a command in VM code */
#define MAX_PATH 1024 /* Max length of the name of entry file */



int type_check(char *);
int parser(char *, char *);
void itoa(long int, unsigned char *);
void reverse(unsigned char *);
void mini_parser(char *, char *, char *, char *);
void writeBootstrap(void);


enum {COMMENT, NEW_LINE, INSTRUCTION};

FILE *fp, *fp2;
int SP = STACK_BASE;	/* stack pointer */
char current_function[MAXCHAR] = {0}; /* current function name */
int number_of_calls = 0; 	/* how many calls in the paricular functions so far */
char current_file[MAXCHAR] = {0}; /* current file name */

int main(int argc, char *argv[]) {

	char *prog = argv[0];	/* program name for errors */
	int name_length;	/* length of the filename */
	char file_to_open[MAX_PATH] = {0}; // Initialize all elements to '\0\'

/* Dealing with files */
	if (argc < 2) {
		fprintf(stderr, "%s: Creates assembly version of VM code\nUsage: prompt>VM_Translator Program.vm\n",prog+2);
		exit(1);
	}

	struct stat path_stat;
	if (stat(argv[1], &path_stat) == 0) {
		// if entry is a directory
		if (S_ISDIR(path_stat.st_mode)) {
			printf("%s is a directory, now rolling:\n", argv[1]);

			// Set directory path from command line argument
			// If the path doesn't end with '/', add it.
			char directoryPath[MAX_PATH] = {0}; // Initialize all elemts to '\0'
			strcat(directoryPath, argv[1]);
			if (directoryPath[strlen(directoryPath) - 1] != '/')
				directoryPath[strlen(directoryPath)] = '/';

			/* Creating the file to write the assembly code */
			strcat(file_to_open, directoryPath);
			// Delete '/' and add .asm to create the name of the file to open
			file_to_open[strlen(file_to_open) - 1] = '\0';
			strcat(file_to_open, ".asm");
			
			if ((fp2 = fopen(file_to_open, "w")) == NULL) {
				fprintf(stderr, "%s can't create %sXX\n",
						prog+2, file_to_open);
				exit(4);
			}
			char line[MAXCHAR], *lineptr, line_asm[MAXCHAR];
			lineptr = line;
			int value_type;
			/* Stack initialization */
			writeBootstrap();

			DIR *dir = opendir(directoryPath);
			if (dir == NULL) {
				fprintf(stderr, "can't open %s\n", argv[1]);
				exit(1);
			}

			
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL) {
				// Skip "." and ".." entries
				if ((strcmp(entry->d_name, ".") == 0) || strcmp(entry->d_name, "..") == 0)
					continue;

				// Build the full  path to the entry
				char fullPath[MAX_PATH];
				snprintf(fullPath, sizeof(fullPath), "%s%s", directoryPath, entry->d_name);

				struct stat path_stat;
				stat(fullPath, &path_stat);
				
				// Check if entry is a regular file with ".vm" suffix
				if (S_ISREG(path_stat.st_mode) && strstr(entry->d_name, ".vm") != NULL &&
						strcmp(entry->d_name + strlen(entry->d_name) - 3, ".vm") == 0 && *entry->d_name != '.') {
					printf("Processing file %s\n", entry->d_name);
					strncpy(current_file, entry->d_name, strlen(entry->d_name) - 3);
					current_file[strlen(entry->d_name) - 3] = '\0';
					printf("XXXXXXXXCurrent_file:%sXXX\n", current_file);
					if ((fp = fopen(fullPath, "r")) == NULL) {
						fprintf(stderr, "%s can't open %s\n",
							prog+2, argv[1]);
						exit(4);
						}
					/* Translating VM files into Assembly file */
					while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
						value_type = type_check(line);
						if (value_type == INSTRUCTION) {
							parser(line, line_asm);
				//			printf("INSTRUCTION: %s", line);
							// if (fpust(line_asm, fp2) == EOF } ...
						}
						else if (value_type == COMMENT) {
				//			printf("COMMENT: %s", line);
						}
						else {
				//			printf("A NEW LINE: %s", line);
						}

					}	


				}
				else
					printf("%s is not a regular file or lack .vm suffix\n", entry->d_name);
			}

		}



		else if (S_ISREG(path_stat.st_mode)) {
			printf("%s is a file\n", argv[1]);
			if (!(strstr(argv[1], ".vm") != NULL && strcmp(argv[1] + strlen(argv[1]) - 3, ".vm") == 0) && *argv[1] == '.') {
				fprintf(stderr, "%s has no .vm suffix or is invalid\n", argv[1]);
				exit(1);
			}
			// creating the output file
			strncpy(file_to_open, argv[1], strlen(argv[1]) - 3);
			strcpy(current_file, file_to_open);
			strcat(file_to_open, ".asm");
			
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
		writeBootstrap();

		/* Translating VM files into Assembly file */
			while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
				value_type = type_check(line);
				if (value_type == INSTRUCTION) {
					parser(line, line_asm);
//					printf("INSTRUCTION: %s", line);
					// if (fpust(line_asm, fp2) == EOF } ...
				}
				else if (value_type == COMMENT) {
//					printf("COMMENT: %s", line);
				}
				else {
//					printf("A NEW LINE: %s", line);
				}

			}

		}
		else
			printf("%s is something from alines\n", argv[1]);
	}
	else {
		printf("error, can't figure out what %s is\n", argv[1]);
		return EXIT_FAILURE;
	}




	printf("EXIT1\n");
/* Final assembly instructions */
	if (fputs("(END)\n@END\n0;JMP", fp2) == EOF) {
		printf("fputs 5 error\n");
		exit(5);
	}
	printf("EXIT2\n");
	exit(0);
}

/* Because NestCall test is kinda retarded, this strange bootstrap code (with INITIALIZED variable) has to be applied */
void writeBootstrap(void) {
    // Check if SP is already set
    if (fputs("@SP\nD=M\n@INITIALIZED\nD;JNE\n", fp2) == EOF) 
		exit(5);

    // Initialize SP to 256 if not already set
    if (fputs("@256\nD=A\n@SP\nM=D\n", fp2) == EOF) 
		exit(5);

    // Mark initialization completed
    if (fputs("(INITIALIZED)\n", fp2) == EOF) 
		exit(5);

	// Call Sys.init
	if (fputs("@Sys.init$ret1\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
		exit(5);

	if (fputs("@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push LCL
    	exit(5);    
	if (fputs("@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push ARG
    	exit(5);
	if (fputs("@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push THIS
    	exit(5);
	if (fputs("@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push THAT
    	exit(5);


	if (fputs("@SP\nD=M\n@5\nD=D-A\n", fp2) == EOF) // ARG = SP - 5 - numArgs
    	exit(5);
	if (fputs("@0\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@Sys.init\n0;JMP\n", fp2) == EOF)
		exit(5);

	// Return label
	if (fputs("(Sys.init$ret1)\n", fp2) == EOF)
		exit(5);
		
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



/* integers for labels for comparasion commands (eq, gt, lt) */
int LABEL_NO = 0;
int EXIT_NO = 0;

#define MAX_NAME 100 /* max length of the name of a single part of the command */
int parser(char *line_vm, char *line_asm) {

		char label_number[100], label_number2[100], exit_number[100], stack_number[100], assbl_inst[MAX_PATH];
		char *string_ptr = line_vm;
		int i = 0;
		
		// Set up the parts of the name 
		char name1[MAX_NAME], name2[MAX_NAME], name3[MAX_NAME];
		mini_parser(line_vm, name1, name2, name3);
		printf("VM_COMMANDS:%s %s %s\n", name1, name2, name3);
		if (strstr(name1, "push")) {
			if (fputs("@", fp2) == EOF)
				exit(5);
			if (fputs(name3, fp2) == EOF)
				exit(5);


			if (strstr(name2, "constant")) {
				if (fputs("\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) { //Checked
					exit(5);
				}
			}
			 
			else if (strstr(name2, "local")) {
				if (fputs("\nD=A\n@LCL\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("push:lcl \n");
			}
			else if (strstr(name2, "this")) {
				if (fputs("\nD=A\n@THIS\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("push:this \n");
			}
			else if (strstr(name2, "that")) {
				if (fputs("\nD=A\n@THAT\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("push:that \n");
			}
			else if (strstr(name2, "temp")) {
				if (fputs("\nD=A\n@5\nA=A+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("push:temp \n");
			}
			else if (strstr(name2, "argument")) {
				if (fputs("\nD=A\n@ARG\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
					exit(5);
				printf("push:arg \n");
			}

			/* pointers */
			else if (strstr(name2, "pointer")) {
				if (strstr(name3, "0")) {
					if (fputs("\n@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
						exit(5);
				}
				else if (strstr(name3, "1")) {
					if (fputs("\n@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
						exit(5);
				}
			}

			else if (strstr(name2, "static")) {
				snprintf(assbl_inst, MAX_PATH, "\n@%s.%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", current_file, name3);
				if (fputs(assbl_inst, fp2) == EOF)
					exit(5);
			}

		}
		else if (strstr(name1, "pop")) {
		
			/* In order to save (pop) the top value of the stack into a segment memory(local, argument, ...), save the index part of the segment into virtual memory,
			 * then get the base address of the segment, next add the base address and index, finally save the stack top value into base+index segment of the memory(local, argument, ...) */
			
			/* get the value */
			if (fputs("@", fp2) == EOF)
				exit(5);
			if (fputs(name3, fp2) == EOF)
			exit(5);
			

			if (strstr(name2, "argument")) {
				if (fputs("\nD=A\n@R13\nM=D\n@ARG\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
			}
			else if (strstr(name2, "local")) {
				if (fputs("\nD=A\n@R13\nM=D\n@LCL\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("pop:loc  \n");
			}
			else if (strstr(name2, "this"))  {
				if (fputs("\nD=A\n@R13\nM=D\n@THIS\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("pop:this  \n");
			}
			else if (strstr(name2, "that")) {
				if (fputs("\nD=A\n@R13\nM=D\n@THAT\nD=M\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("pop:that  \n");
			}
			else if (strstr(name2, "temp")) {
				if (fputs("\nD=A\n@R13\nM=D\n@5\nD=A\n@R13\nM=M+D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n", fp2) == EOF)
				exit(5);
				printf("pop:temp  \n");
			}
			else if (strstr(name2, "pointer")) {
				if (strstr(name3, "0")) {
					if (fputs("\n@SP\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n", fp2) == EOF)
						exit(5);
				}
				else if (strstr(name3, "1")) {
					if (fputs("\n@SP\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n", fp2) == EOF)
						exit(5);
				}
			}
			else if (strstr(name2, "static")) {
				snprintf(assbl_inst, MAX_PATH, "\n@SP\nM=M-1\nA=M\nD=M\n@%s.%s\nM=D\n", current_file, name3);
				if (fputs(assbl_inst, fp2) == EOF)
					exit(5);

			}
		}



	else if (strstr(name1, "add")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M+D\n@SP\nM=M+1\n", fp2) == EOF)   
			exit(5);
	}
	else if (strstr(name1, "sub")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M-D\n@SP\nM=M+1\n", fp2) == EOF) 
			exit(5);
	}
	else if (strstr(name1, "neg")) {
		if (fputs("@SP\nA=M-1\nM=-M\n", fp2) == EOF)  
			exit(5);
	}
	else if (strstr(name1, "or")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M|D\n@SP\nM=M+1\n", fp2) == EOF) 
			exit(5);
	}
	else if (strstr(name1, "and")) {
		if (fputs("@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D&M\n@SP\nM=M+1\n", fp2) == EOF)
			exit(5);
	}
	else if (strstr(name1, "not")) {
		if (fputs("@SP\nA=M-1\nM=!M\n", fp2) == EOF)
			exit(5);
	}



	else if (strstr(name1, "eq") || strstr(name1, "lt") || strstr(name1, "gt")) {
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
		if (strstr(name1, "eq")) {
			if (fputs("\nD;JEQ\n", fp2) == EOF)
				exit(5);
		}
		else if (strstr(name1, "lt")) {
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
		if (fputs(")\n@SP\nA=M\nM=0\n@exit", fp2) == EOF)
			exit(5);
		if (fputs(exit_number, fp2) == EOF)
			exit(5);
		if (fputs("\n0;JMP\n(label", fp2) == EOF)
			exit(5);
		if (fputs(label_number2, fp2) == EOF)
			exit(5);
		if (fputs(")\n@SP\nA=M\nM=-1\n@exit", fp2) == EOF)
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

	else if (strstr(name1, "label")) {
		if (strlen(current_function) > 0) 
			snprintf(assbl_inst, MAX_PATH, "(%s$%s)\n", current_function, name2);
		else 
			snprintf(assbl_inst, MAX_PATH, "(%s)\n", name2);
					
		if (fputs(assbl_inst, fp2) == EOF)
			exit(5);

	}
	else if (strstr(name1, "if-goto")) {
		if (strlen(current_function) > 0)
			snprintf(assbl_inst, MAX_PATH, "@SP\nM=M-1\nA=M\nD=M\n@%s$%s\nD;JEQ\n", current_function, name2);
		else
			snprintf(assbl_inst, MAX_PATH, "@SP\nM=M-1\nA=M\nD=M\n@%s\nD;JEQ\n", name2);
		if (fputs(assbl_inst, fp2) == EOF)
			exit(5);
		}
	else if (strstr(name1, "goto")) {
		if (strlen(current_function) > 0)
			snprintf(assbl_inst, MAX_PATH, "@%s$%s\n0;JMP\n", current_function, name2);
		else
			snprintf(assbl_inst, MAX_PATH, "@%s\n0;JMP\n", name2);
		if (fputs(assbl_inst, fp2) == EOF)
			exit(5);
	}


	else if (strstr(name1, "function")) {
		number_of_calls = 0;	
	    if (!(sprintf(assbl_inst, "(%s)\n", name2)))
    	    exit(4);
	    if (fputs(assbl_inst, fp2) == EOF)
    	    exit(5);
	    // Initialize local variables to 0
    	int localVarCount = atoi(name3);
	    for (int i = 0; i < localVarCount; ++i) {
    	    if (fputs("@0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF)
        	    exit(5);
	   	 }
		// set the current function
		strncpy(current_function, name2, strlen(name2) + 1);
	}

	else if (strstr(name1, "call")) {
		++number_of_calls;
	    if (!(sprintf(assbl_inst, "@%s$ret%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", current_function, number_of_calls)))
			exit(5);
		if (fputs(assbl_inst, fp2) == EOF)
  		    exit(5);

	    if (fputs("@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push LCL
    	    exit(5);
	    if (fputs("@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push ARG
    	    exit(5);
	    if (fputs("@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push THIS
    	    exit(5);
	    if (fputs("@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fp2) == EOF) // Push THAT
    	    exit(5);



	    if (fputs("@SP\nD=M\n@5\nD=D-A\n", fp2) == EOF) // ARG = SP - 5 - numArgs
    	    exit(5);
	    if (!(sprintf(assbl_inst, "@%s\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@%s\n0;JMP\n", name3, name2)))
    	    exit(4);
	    if (fputs(assbl_inst, fp2) == EOF)
    	    exit(5);

	    // Return label
    	if (!(sprintf(assbl_inst, "(%s$ret%d)\n", current_function, number_of_calls)))
        	exit(4);
	    if (fputs(assbl_inst, fp2) == EOF)
    	    exit(5);
	
		// set the current funciton
		strncpy(current_function, name2, strlen(name2) + 1);
	}

	else if (strstr(name1, "return")) {
    	// Save the frame's base address
	    if (fputs("@LCL\nD=M\n@frame\nM=D\n", fp2) == EOF)
    	    exit(5);

	    // Save the return address
    	if (fputs("@5\nA=D-A\nD=M\n@retAddr\nM=D\n", fp2) == EOF)
        	exit(5);

	    // Reposition SP for the caller
    	if (fputs("@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n", fp2) == EOF) // *ARG = pop()
        	exit(5);
	    if (fputs("@ARG\nD=M+1\n@SP\nM=D\n", fp2) == EOF) // SP = ARG + 1
    	    exit(5);

		if (fputs("@frame\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n", fp2) == EOF) // Restore THAT
    		exit(5);
		if (fputs("@frame\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n", fp2) == EOF) // Restore THIS
    		exit(5);
		if (fputs("@frame\nM=M-1\nA=M\nD=M\n@ARG\nM=D\n", fp2) == EOF) // Restore ARG
    		exit(5);
		if (fputs("@frame\nM=M-1\nA=M\nD=M\n@LCL\nM=D\n", fp2) == EOF) // Restore LCL
    		exit(5);

	    // Jump to the return address
    	if (fputs("@retAddr\nA=M\n0;JMP\n", fp2) == EOF)
        	exit(5);
	}

	return EXIT_SUCCESS;
}

void mini_parser(char *command, char *name1, char *name2, char *name3) {


	while (isspace(*command))
		++command;
	while (!(isspace(*command)))
		*name1++ = *command++;
	*name1 = '\0';
	while (isspace(*command) && *command != '\n')
		++command;
	while (!(isspace(*command)))
		*name2++ = *command++;
	*name2 = '\0';
	while (isspace(*command) && *command != '\n')
		++command;
	while (!(isspace(*command)))
		*name3++ = *command++;
	*name3 = '\0';
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
