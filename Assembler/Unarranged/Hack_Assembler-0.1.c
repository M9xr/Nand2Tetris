/* Assembler for project 06 - Building completly functional assembler for HACK computer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
struct symbol {
	struct symbol *next;
	char *word;
	int word_value;
} table[] {
	"R0", 0,
	"R1", 1,
	"R2", 2,
	"R3", 3,
	"R4", 4,
	"R5", 5,
	"R6", 6,
	"R7", 7,
	"R8", 8,
	"R9", 9,
	"R10", 10,
	"R11", 11,
	"R12", 12,
	"R13", 13,
	"R14", 14,
	"R15", 15,
	"SP", 0,
	"LCL", 1,
	"ARG", 2,
	"THIS", 3,
	"THAT", 4,
	"SCREEN", 16384,
	"KBD", 24576

}
*/


#define MAX_INST 2500000
#define MAXLINE 16300
#define MAXCHAR 100
#define BITS 18		/* 16 for 0's and 1's, 1 for '\n', and 1 for '\0' */

void filecopy(FILE *ifp, FILE *ofp);
void a_parser(char *, char *);	
void itob(int , char *, int);
void reverse(char *);

int line_no = 0;
/* assembler: parse the asembly code into binary version */
int main (int argc, char *argv[]) {

	FILE *fp, *fp2;
	char *prog = argv[0];	/* program name for errors */
	int namelength;		/* length of the filename */
	char file_to_open[100];

	if (argc != 2) {
		fprintf(stderr, "%s: Creates binary version of assembly code\nUsage: prompt>HackAssembler Program.asm\n", prog+2);
		exit(1);
	}
	namelength = strlen(argv[1]);
	/* check if the name of the file is valid */
	if (strcmp (".asm", argv[1]+namelength-4)){
		fprintf(stderr, "%s: error: file %s has no .asm extension\n", prog+2, argv[1]);
		exit(2);
	}
	strncpy(file_to_open, argv[1], namelength-4);
	strcat(file_to_open, ".hack");
	printf("The file to open: %s\n", file_to_open);

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "%s can't open %s\n",
				prog+2, argv[1]);
		exit(3);
	}
	if ((fp2 = fopen(file_to_open, "w")) == NULL) {
		fprintf(stderr, "%s can't create %s\n",
				prog+2, file_to_open);
		exit(4);
	}
	
	char line[MAXCHAR], *lineptr, line_bit[BITS];
	lineptr = line;
	
	/* Test wheter program can read the file properly */
	while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
		if (strncmp("/", line, 1) == 0) {
			printf("A comment:: %s", line);

		}
		else if ((strlen(line) < 3)) {
			printf("A new line:: \n");
		}
		else if (strncmp("@", line, 1) == 0) {
			printf("An A instruction:: %s", line);
			++line_no;
//			printf("----- Before parser the string: %s----------\n", line);
			a_parser(line, line_bit);
			if (fputs(line_bit, fp2) == EOF) {
				fprintf(stderr, "%s can't write to file: %s\n",
						prog+2, file_to_open);
				exit(6);
			}
		}
		else {
			printf("An C instruction:: %s", line);
			++line_no;
		}
	}
	printf("Numeber of lines: %d\n", line_no);
	/*
	if (strlen(line) > 0) {
		fprintf(stderr, "%s can't fully read %s\n", prog+2, argv[1]);
		exit(5);
	}
*/

	//filecopy(fp, fp2);

	exit(0);
}

/* a_parser: parse "A instruction" in string "s" into binary version "sb"*/
void a_parser(char *s, char *sb) {

	if (!(isdigit(s[1]))) {
		printf("No cusz it is: %c  the string:%s  line_no:%d\n", s[1], s, line_no);
		exit(69);
	}
	printf("-----A_PARSER-----string:%s\n", s);
	int number;
	number = atol(s+1);
	itob(number, sb, 2);
	printf("Converted string: %s number: %d\n", sb, number);
}

/* itob: convert integer b into other version (binary, hexadecimal...) */
void itob (int n, char s[], int b) {

	int i, j, c;

	printf("-----ITOB---Tje string: %s base:%d---------\n", s, b);
	j = 0;
	for ( i = n; i > 0; i /= b) {
		if ((c = (i % b)) <= 9)
			s[j++] = c + '0';
		else
			s[j++] = c - 10 + 'A';
	}
	s[j] = '\0';
	printf("-----xxxxxxxxxxITOB---Tje string: %s base:%d---------\n", s, b);
	reverse (s);
}

/* reverse the order in the string */
void reverse (char s[]) {

	int c, i, j;
	for (i = 0, j = strlen(s) -1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
	

/* filecopy: copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp) {

	int c;
	while ((c = getc(ifp)) != EOF)
		putc(c, ofp);
}
