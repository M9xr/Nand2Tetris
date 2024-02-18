/* Assembler for project 06 - Building completly functional assembler for HACK computer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct symbol {
	struct symbol *next;	/* next entry in chain */
	char *name;		/* defined name */	
	int defn_val;		/* value for the defined name */
};


/* Table to initialize */
struct to_init {
	char *word;		
	int word_value;
} table[] = {
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

};


#define MAXCHAR 100	/* max length of the word */
#define BITS 18		/* 16 for 0's and 1's, 1 for '\n', and 1 for '\0' */
#define BITS_BASIC 16	/* for a_parser */
#define VAR_START 16	/* Start RAM space address for storing variables */
#define HASHSIZE 101	/* size of the symbol table */
static struct symbol *hashtab[HASHSIZE];	/* pointer table */

void a_parser(char *, char *);	
void c_parser(char *, char *);	
void itob(int , char *, int);
void reverse(char *);

struct symbol *install(char *, int);
struct symbol *lookup(char *);
unsigned hash(char *);
char *xstrdup(char *);
void print_hash(void);
int type_check(char *, char *);	


int line_no = 0;	/* line number */
int var_ram = VAR_START;	/* Variable for RAM space address for storing variables */

enum {COMMENT, LABEL, NEW_LINE, A_INST, C_INST};

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


	/* Table initialization */	
	int i, table_len;
	table_len = sizeof(table) / sizeof(table[0]);
	for (i = 0; i < table_len; ++i)
		install(table[i].word, table[i].word_value);

	
	char line[MAXCHAR], *lineptr, line_bit[BITS], line2[MAXCHAR];
	lineptr = line;
	int string_length, value_type;
	char string2[MAXCHAR];
/* First run, add label symbol to the table */
	while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
		value_type = type_check(line, line2);
		if (value_type == LABEL) {
                        install(line2, line_no);
                }
	}
	
	if (fseek(fp, 0, 0)) {
		fprintf(stderr, "%s can't read file: %s\n",
				prog+2, file_to_open);
		exit(666);
	}
			


/* Second run, parse the code and add variables to the table */
	while ((lineptr = fgets(line, MAXCHAR, fp)) != NULL) {
		value_type = type_check(line, line2);
		if (value_type == A_INST) {
			a_parser(line2, line_bit);
			if (fputs(line_bit, fp2) == EOF) {
				fprintf(stderr, "%s can't write to file: %s\n",
						prog+2, file_to_open);
				exit(6);
			}
		}
		else if (value_type == LABEL) 
			;
		else if (value_type == C_INST) {
			c_parser(line2, line_bit);
			if (fputs(line_bit, fp2) == EOF) {
				fprintf(stderr, "%s can't write to file: %s\n",
						prog+2, file_to_open);
				exit(7);
			}
		}
	//	else
//			printf("Ehh.. STRING: %s\n", line);
	}
	printf("Number of lines: %d\n", line_no);

//	print_hash();

	exit(0);
}


/* type_check: remove white_spaces from labels and instructions;
 * return type of the instructions */
int type_check(char *s, char *cs) {

	char *pc;
	pc = cs;
	while (isspace(*s))
		++s;
	if (*s == '@') {
		++line_no;
		++s;
		while (isalnum(*s) || *s == '_' || *s == '.' || *s == '$') {
			*cs = *s;
			++cs;
			++s;
		}
		*cs = '\0';
		return A_INST;
	}
	else if (*s == '(') {
		++s;
		while (*s != ')') {
			*cs = *s;
			++cs;
			++s;
		}
		*cs = '\0';
		return LABEL;
	}
	else if (*s == '/') {
		return COMMENT;
	}
	else if (isalnum(*s)) {
		++line_no;
		*cs = *s;
		++cs;
		++s;
		while (!(isspace(*s))){
			*cs = *s;
			++cs;
			++s;
		}
		*cs = '\0';
		return C_INST;
	}
	else {
		return NEW_LINE;
	}
}



/* parse C-instruction */
void c_parser (char *s, char *sb) {

	/* dest = comp;jump
	 * 1 1 1 a c c c c c c d d d j j j 
	 * comp is mandatory. If dest is empty, the "=" is omitted. If jump is empty, the ";" is omitted */


	int i;
	for (i = 0; i < BITS_BASIC; ++i)
		sb[i] = '0';
	sb[0] = sb[1] = sb[2] = '1';
	sb[16] = '\n';
	sb[17] = '\0';

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
	}

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
	}

	/* comp */
	char string1[20];
	if ((p = strstr(s, "=")) != NULL) {
		++p;
		strcpy(string1, p);
		strcpy(s, string1);
	}
	
	char *p3;
	if (((p = strstr(s, "M")) != NULL) && ((p3 = strstr(s, "JMP")) == NULL)) {
		sb[3] = '1';
		*p = 'A';
	}

	/* In order to prevent double check, firstly program checks the longest strings like "D+1", then single chars like "D" */
	if ((p = strstr(s, "D+1")) != NULL) 
		sb[5] = sb[6] = sb[7] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "A+1")) != NULL)
		sb[4] = sb[5] = sb[7] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "D-1")) != NULL)
		sb[6] = sb[7] = sb[8] = '1';
	else if ((p = strstr(s, "A-1")) != NULL)
		sb[4] = sb[5] = sb[8] = '1';
	else if ((p = strstr(s, "D+A")) != NULL) 
		sb[8] = '1';
	else if ((p = strstr(s, "D-A")) != NULL)
		sb[5] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "A-D")) != NULL)
		sb[7] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "D&A")) != NULL)
		;
	else if ((p = strstr(s, "D|A")) != NULL)
		sb[5] = sb[7] = sb[9] = '1';
	else if ((p = strstr(s, "!D")) != NULL)
		sb[6] = sb[7] = sb[9] = '1';
	else if ((p = strstr(s, "!A")) != NULL)
		sb[4] = sb[5] = sb[9] = '1';
	else if ((p = strstr(s, "-D")) != NULL)
		sb[6] = sb[7] = sb[8] = sb[9] = '1'; 
	else if ((p = strstr(s, "-A")) != NULL)
		sb[4] = sb[5] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "-1")) != NULL)
		sb[4] = sb[5] = sb[6] = sb[8] = '1';



	else if ((p = strstr(s, "0")) != NULL)
		sb[4] = sb[6] = sb[8] = '1';
	else if ((p = strstr(s, "1")) != NULL)
		sb[4] = sb[5] = sb[6] = sb[7] = sb[8] = sb[9] = '1';
	else if ((p = strstr(s, "D")) != NULL) 
		sb[6] = sb[7] = '1';
	else if ((p = strstr(s, "A")) != NULL) 
		sb[4] = sb[5] = '1';
	
}
/* a_parser: parse "A instruction" in string "s" into binary version "sb"*/
void a_parser(char *s, char *sb) {

	struct symbol *p1;
	int number, str_length, i;
	char string_second[BITS];
	if (!(isdigit(s[0]))) {
		if ((p1 = lookup(s)) == NULL) {
			p1 = install(s, var_ram);
			number = var_ram;
			++var_ram;
		}
		else {
			number = p1->defn_val;
		}
	}
	else {	
		/* Convert array of decimal numbers into number */
		number = atol(s);
	}
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
	
/* hash: form hash value for string s */
unsigned hash(char *s) {

	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 *hashval;
	return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct symbol *lookup(char *s) {

	struct symbol *np;
	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return np;	/* found */
	return NULL;			/* not found */
}

/* install: put (name, defn) in hashtab */
struct symbol *install(char *name, int defn) {

	struct symbol *np;
	unsigned hashval;

	if ((np = lookup(name)) == NULL) {	/* not found */
		np = (struct symbol *) malloc(sizeof(*np));
		if (np == NULL || (np->name = xstrdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];	/* pointer to next gets the address of the "old" list */
		hashtab[hashval] = np;		/* beginning of the list gets the address of the new struct */
	
		np->defn_val = defn;
	}
//	else /* already there */
//		free((void *) np->defn);	/* free previous defn */
//	return NULL;
	else
		printf("ERROR EORR\n");
	return np;
}


/* xstrdup: make a duplicate of char s */
char *xstrdup(char *s) {

	char *p;
	p = (char *) malloc(strlen(s)+1);	/* +1 for '\0' */
	if (p != NULL)
		strcpy(p, s);
	return p;
}


/* print_hash: print hashed values */
void print_hash(void) {

        struct symbol *p;
        for (int i = 0; i < HASHSIZE; i++) {
                if ((p = hashtab[i]) != NULL) {
                        printf("Hash value of the following words: %d\n", i);
                        for (; p != NULL; p = p->next)
                                printf("|Name: %s Defn: %d|  ", p->name, p->defn_val);
                        printf("\n\n");
                }
        }
}


