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
#define BITS_BASIC 16	/* for a_parser */

void a_parser(char *, char *);	
void c_parser(char *, char *);	
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
//			printf("A comment:: %s", line);

		}
		else if ((strlen(line) < 3)) {
//			printf("A new line:: \n");
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
			c_parser(line, line_bit);
			if (fputs(line_bit, fp2) == EOF) {
				fprintf(stderr, "%s can't write to file: %s\n",
						prog+2, file_to_open);
				exit(7);
			}
		}
	}
	printf("Number of lines: %d\n", line_no);


	exit(0);
}


void c_parser (char *s, char *sb) {

	/* dest = comp;jump
	 * 111 a c c c c c c d d d j j j 
	 * comp is mandatory. If dest is empty, the "=" is omitted. If jump is empty, the ";" is omitted */


	int i;
	for (i = 0; i < BITS_BASIC; ++i)
		sb[i] = '0';
	sb[0] = sb[1] = sb[2] = '1';
	sb[16] = '\n';
	sb[17] = '\0';

	printf("--------Sb1: %s\n", sb);
	printf("----------------THE S STRING: %s\n", s);
	char *p, *p2;
	/* dest */
	char string2[20];
	if ((p = strstr(s, "=")) != NULL) {
		for (i = 0; i < 20 & s[i] != '='; ++i)
			string2[i] = s[i];
		string2[i] = '\0';
		if ((p = strstr(string2, "M")) != NULL)
			sb[12] = '1';
		if ((p = strstr(string2, "D")) != NULL)
			sb[11] = '1';
		if ((p = strstr(string2, "A")) != NULL)
			sb[10] = '1';
		printf("-----Test (=)\n");
	}
	else
		printf("JAK KREW W PIACH\n");
	printf("--------Sb2: %s\n", sb);
	/* jump */
	if ((p = strstr(s, ";")) != NULL) {
		if ((p = strstr(s, "JGT"))!= NULL)
			sb[15] = '1';
		if ((p = strstr(s, "JEQ")) != NULL)
			sb[14] = '1';
		if ((p = strstr(s, "JGE")) != NULL)
			sb[14] = sb[15] = '1';
		if ((p = strstr(s, "JLT")) != NULL)
			sb[13] = '1';
		if ((p = strstr(s, "JNE")) != NULL)
			sb[13] = sb[15] = '1';
		if ((p = strstr(s, "JLE")) != NULL)
			sb[13] = sb[14] = '1';
		if ((p = strstr(s, "JMP")) != NULL)
			sb[13] = sb[14] = sb[15] = '1';
		printf("-----Test (;)\n");
	}

	printf("--------Sb3: %s\n", sb);
	/* comp */

	/* -----------
	 * There is double, triple... check on letters...
	 * repair!
	 *  you can just inser the biggest strings into first's places and make "else if" for further strings*/
	char string1[20];
	if ((p = strstr(s, "=")) != NULL) {
		++p;
		strcpy(string1, p);
		strcpy(s, string1);
		printf("***CO***\n");
	}

	if ((p = strstr(s, "M")) != NULL) {
		sb[3] = '1';
		printf("**** WE HAVE THE \"M\"\n");
	}
	if ((p = strstr(s, "0")) != NULL)
		sb[4] = sb[6] = sb[8] = '1';
	if ((p = strstr("1", s)) != NULL)
		sb[4] = sb[5] = sb[6] = sb[7] = sb[8] = sb[9] = '1';
	if ((p = strstr(s, "-1")) != NULL)
		sb[4] = sb[5] = sb[6] = sb[8] = '1';
	if ((p = strstr(s, "D")) != NULL && (p2 = strstr(s, "D-A") == NULL)) {
		sb[6] = sb[7] = '1';
		printf("SIEMA D Z TEJ STORNYH\n");
	}
	if ((p = strstr(s, "A")) != NULL) {
		sb[4] = sb[5] = '1';
		printf("ZLEEEE :DDDDDDDDDDDDDDDDDDDD\n");
	}
	if ((p = strstr(s, "!D")) != NULL)
		sb[6] = sb[7] = sb[9] = '1';
	if ((p = strstr(s, "!A")) != NULL)
		sb[4] = sb[5] = sb[9] = '1';
	if ((p = strstr(s, "-D")) != NULL)
		sb[6] = sb[7] = sb[8] = sb[9] = '1'; 
	if ((p = strstr(s, "-A")) != NULL)
		sb[4] = sb[5] = sb[8] = sb[9] = '1';
	if ((p = strstr(s, "D+1")) != NULL) {
		sb[5] = sb[6] = sb[7] = sb[8] = sb[9] = '1';
		printf("****WSZYSTKO JEST JUZ NICZYMAAAAAAAAAAAAAAA AAA \n");
	}
	if ((p = strstr(s, "A+1")) != NULL)
		sb[4] = sb[5] = sb[7] = sb[8] = sb[9] = '1';
	if ((p = strstr(s, "D-1")) != NULL)
		sb[6] = sb[7] = sb[8] = '1';
	if ((p = strstr(s, "A-1")) != NULL)
		sb[4] = sb[5] = sb[8] = '1';
	if ((p = strstr(s, "D+A")) != NULL) {
		sb[8] = '1';
		printf("JAAAAJA AJAJAJ JAJAAAAAAAAAAAAAAAAAAA \n");
	}
	if ((p = strstr(s, "D-A")) != NULL)
		sb[5] = sb[8] = sb[9] = '1';
	if ((p = strstr(s, "A-D")) != NULL)
		sb[7] = sb[8] = sb[9] = '1';
	if ((p = strstr(s, "D&A")) != NULL)
		;
	if ((p = strstr(s, "D|A")) != NULL)
		sb[5] = sb[7] = sb[9] = '1';
		
		

	

	printf("--------Sb4: %s\n", sb);
	printf("Cza-cza\n");
}
/* a_parser: parse "A instruction" in string "s" into binary version "sb"*/
void a_parser(char *s, char *sb) {

	if (!(isdigit(s[1]))) {
		printf("No cusz it is: %c  the string:%s  line_no:%d\n", s[1], s, line_no);
		exit(69);
	}
	int number, str_length, i;
	char string_second[BITS];
	
	/* Convert array of decimal numbers into number */
	number = atol(s+1);
	/* Convert decimal number into binary */
	itob(number, string_second, 2);
	/* length of the array of binary values */
	str_length = strlen(string_second);
	/* Set left bits to 0's */
	for (i = 0; i < (BITS_BASIC - str_length); ++i)
		sb[i] = '0';
	sb[i] = '\0';
	/* concatenate string with value with string that has 0's to make proper 16-bit value*/
	strcat(sb, string_second);
	/* write a new line to the string in order to easily fit it into a new file */
	sb[16] = '\n';
	sb[17] = '\0';

	



	printf("Converted string: %s number: %d  str_length:%d\n", sb, number, str_length);
}

/* itob: convert integer b into other version (binary, hexadecimal...) */
void itob (int n, char s[], int b) {

	int i, j, c;

	j = 0;
	for ( i = n; i > 0; i /= b) {
		if ((c = (i % b)) <= 9)
			s[j++] = c + '0';
		else
			s[j++] = c - 10 + 'A';
	}
	s[j] = '\0';
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
	

