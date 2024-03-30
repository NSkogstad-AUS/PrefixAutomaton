/* Program to perform multi-digit integer arithmetic.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2023, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.
   All included code is (c) Copyright University of Melbourne, 2023

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [NICOLAI SKOGSTAD]
   Dated:     [15/09/2023]

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

/* All necessary #defines provided as part of the initial skeleton */

#define INTSIZE	500	/* max number of digits per integer value */
#define LINELEN	999	/* maximum length of any input line */
#define NVARS	26	/* number of different variables */

#define CH_A     'a'    /* character 'a', first variable name */

#define ERROR	(-1)	/* error return value from some functions */
#define PROMPT	"> "	/* the prompt string for interactive input */

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define MULT	'*'	/* the multiplication operator */
#define POWR	'^'	/* the power-of operator */
#define DIVS	'/'	/* the division operator */
#define ALLOPS  "?=+*^/"

#define CH_ZERO  '0'    /* character zero */
#define CH_ONE   '1'    /* character one */
#define CH_NINE  '9'    /* character nine */

#define CH_COM   ','    /* character ',' */
#define PUT_COMMAS 3    /* interval between commas in output values */

#define INT_ZERO 0	/* integer 0 */
#define INT_TEN  10	/* integer 10 */

/* Placeholder typedef for skeleton code
*/
typedef int longint_t;
#define LONG_ZERO 0
#define LONG_ONE  1


/****************************************************************/

/* A "magic" additional function needing explicit declaration */
int fileno(FILE *);

/* Skeleton program function prototypes */

void print_prompt(void);
void print_tadaa();
void print_error(char *message);
int  read_line(char *line, int maxlen);
void process_line(longint_t vars[][INTSIZE+1], char *line);
int  get_second_value(longint_t *vars, char *rhsarg,
	longint_t *second_value);
int  to_varnum(char ident);
void do_print(int varnum, longint_t *var);
void do_assign(longint_t *var1, longint_t *var2);
void do_plus(longint_t *var1, longint_t *var2);
void zero_vars(longint_t vars[][INTSIZE+1]);
void parse_num(char *rhs, longint_t *secondvalArr);
void do_mult(longint_t *var1, longint_t *var2);
int commaSep(int n);



/****************************************************************/

/* Main program controls all the action
*/
int
main(int argc, char *argv[]) {
	char line[LINELEN+1] = {0};
	longint_t vars[NVARS][INTSIZE+1];

	zero_vars(vars);
	print_prompt();
	while (read_line(line, LINELEN)) {
		if (strlen(line) > 0) {
			/* non empty line, so process it */
			process_line(vars, line);
		}
		print_prompt();
	}

	print_tadaa();
	return 0;
}

/****************************************************************/

/* Set the vars array to all zero values
*/
void zero_vars(longint_t vars[][INTSIZE + 1]) {
    int i, j;
    longint_t zero = 0LL;  // Initialize with zero
    for (i = 0; i < NVARS; i++) {
        for (j = 0; j <= INTSIZE; j++) {
            vars[i][j] = zero;
        }
    }
}


/* Prints the prompt indicating ready for input, but only if it
   can be confirmed that the input is coming from a terminal.
   Plus, output might be going to a file, that's why the prompt,
   if required, is written to stderr and not stdout
*/
void
print_prompt(void) {
	if (isatty(fileno(stdin))) {
		fprintf(stderr, "> ");
		fflush(stderr);
	}
}

void
print_tadaa() {
	/* all done, so pack up bat and ball and head home,
	   getting the exact final lines right is a bit tedious,
	   because input might be coming from a file and output
	   might be going to a file */
	if (isatty(fileno(stdin)) && isatty(fileno(stdout))) {
		printf("\n");
	}
	printf("ta daa!!!\n");
	if (isatty(fileno(stdin)) && !isatty(fileno(stdout))) {
		fprintf(stderr, "\n");
	}
}

void
print_error(char *message) {
	/* need to write an error message to the right place(s)
	*/
	if (isatty(fileno(stdin)) || isatty(fileno(stdout))) {
		fprintf(stderr, "%s\n", message);
		fflush(stderr);
	}
	if (!isatty(fileno(stdout))) {
		printf("%s\n", message);
	}
}

/****************************************************************/

/* Reads a line of input into the array passed as argument,
   returns false if there is no input available.
   All whitespace characters are removed on the way through.
*/
int
read_line(char *line, int maxlen) {
	int i=0, c;
	while (((c=getchar())!=EOF) && (c!='\n')) {
		if (i<maxlen && !isspace(c)) {
			line[i++] = c;
		}
	}
	line[i] = '\0';
	/* then, if the input is coming from a file or the output
	   is going to a file, it is helpful to echo the input line
	   and record what the command was */
	if (!isatty(fileno(stdin)) || !isatty(fileno(stdout))) {
		printf("%s%s\n", PROMPT, line);
	}
	return ((i>0) || (c!=EOF));
}

/****************************************************************/

/* Process a command by parsing the input line into parts
*/
void
process_line(longint_t vars[][INTSIZE+1], char *line) {
	int varnum, status;
	longint_t second_value[INTSIZE+1];
    char optype;

	/* determine the LHS variable, it
	   must be first character in compacted line
	*/
	varnum = to_varnum(line[0]);
	if (varnum==ERROR) {
		print_error("invalid LHS variable");
		return;
	}

	/* more testing for validity 
	*/
	if (strlen(line)<2) {
		print_error("no operator supplied");
		return;
	}

	/* determine the operation to be performed, it
	   must be second character of compacted line
	*/
	optype = line[1];
	if (strchr(ALLOPS, optype) == NULL) {
		print_error("unknown operator");
		return;
	}

	/* determine the RHS argument (if one is required),
	   it must start in the third character of compacted line
	*/
	if (optype != PRINT) {
		if (strlen(line)<3) {
			print_error("no RHS supplied");
			return;
		}
		status = get_second_value(*vars, line+2, second_value);
		if (status==ERROR) {
			print_error("RHS argument is invalid");
			return;
		}
	}

	/* finally, do the actual operation
	*/
    
	if (optype == PRINT) {
		do_print(varnum, &vars[varnum]);
	} else if (optype == ASSIGN) {
		do_assign(&vars[varnum], second_value);
	} else if (optype == PLUS) {
		do_plus(&vars[varnum], second_value);
	// you will need to add further operators here
	} else if (optype == MULT) {
	    do_mult(&vars[varnum], second_value);
	} else {
		print_error("operation not available yet");
		return;
	}
	return;
}

/****************************************************************/

/* Convert a character variable identifier to a variable number
*/
int
to_varnum(char ident) {
	int varnum;
	varnum = ident - CH_A;
	if (0<=varnum && varnum<NVARS) {
		return varnum;
	} else {
		return ERROR;
	}
}

/****************************************************************/

/* Process the input line to extract the RHS argument, which
   should start at the pointer that is passed
*/
int
get_second_value(longint_t *vars, char *rhsarg,
			longint_t *second_value) {
	char *p;
	int varnum2;
	if (isdigit(*rhsarg)) {
		/* first character is a digit, so RHS is a number
		   now check the rest of RHS for validity */
		for (p=rhsarg+1; *p; p++) {
			if (!isdigit(*p)) {
				/* nope, found an illegal character */
				return ERROR;
			}
		}
		/* nothing wrong, ok to convert */
		parse_num(rhsarg, second_value);
		return !ERROR;
	} else {
		/* argument is not a number, so should be a variable */
		varnum2 = to_varnum(*rhsarg);
		if (varnum2==ERROR || strlen(rhsarg)!=1) {
			/* nope, not a variable either */
			return ERROR;
		}
		/* and finally, get that variable's value */
		do_assign(second_value, &vars[varnum2]);
		return !ERROR;
	}
	return ERROR;
}


/*****************************************************************
******************************************************************

Your answer to the assignment should start here, using your new
typedef defined at the top of the program. The next few functions
will require modifications because of the change of structure
used for a long_int, and then you'll need to start adding whole
new functions after you get these first ones working properly.
Try and make the new functions fit the style and naming pattern
of the existing ones, ok?

******************************************************************
*****************************************************************/

/* Create an internal-format number out of a string
*/
void parse_num(char *rhs, longint_t *secondvalArr) {
    // Setting the initial variables we'll be concerned with

	int initialArr[INTSIZE];
	longint_t finishedArr[INTSIZE+1];
	int rhslength = strlen(rhs);
    
    // Setting the array's values to be -1 so no unwanted integer values
    for (int i = 0; i < INTSIZE; i++) {
        initialArr[i] = -1; 
        finishedArr[i] = -1; }
        
    // Put all the integer values on the RHS into an initial array
	for (int i = 0; i < rhslength; i++) {
		initialArr[i] = rhs[rhslength - i - 1] - '0'; }
		
	// Putting it into another array that is offset by 1 to accomodate for digit count
	for (int j = 0; j < rhslength; j++) {
		finishedArr[j+1] = initialArr[j]; }
    finishedArr[0] = rhslength;


    // Putting the values back into the secondvalArr
	for (int i = 0; i < (INTSIZE+1); i++) {
        secondvalArr[i] = 0; }
    for (int i = 0; i < (INTSIZE+1); i++) {
        secondvalArr[i] = finishedArr[i]; }

}

/****************************************************************/

/* Print out a longint value
*/
void do_print(int varnum, longint_t *var) {
    printf("register %c: ", varnum + 'a');
    
    int length = var[0];
    int comma = (length - 1) % 3 + 1;

    for (int i = length; i >= 1; i--) {
        printf("%d", var[i]);
        if (i > 1 && (i - 1) % 3 == 0) {
            printf(",");
        }
    }
    
    printf("\n");
}

/****************************************************************/

/* Assign a longint value, could do this with just an assignment
   statement, because structs can be assigned, but this is more
   elegant, and only copies over the array elements (digits) that
   are currently in use: var1 = var2

*/
void
do_assign(longint_t *var1, longint_t *var2) {
    for (int i = 0; i < INTSIZE+1; i++) {
        var1[i] = var2[i];
    }
}

/****************************************************************/

/* Update the indicated variable var1 by doing an addition
   using var2 to compute var1 = var1 + var2
*/
void splitInteger(int num, int splitValues[], int* digit_count) {
    // creating a copy to avoid modifying original value
    int num_copy = num;
    
    // initalizing to value of zero
    *digit_count = 0;

    // Count the number of digits in the integer
    while (num_copy > 0) {
        num_copy /= 10;
        (*digit_count)++;
    }

    // set the copy back to the original value
    num_copy = num;

    // Extract and store each digit in the array in reverse order
    for (int i = (*digit_count) - 1; i >= 0; i--) {
        splitValues[i] = num_copy % 10;
        num_copy /= 10;
    }
}

void
do_plus(longint_t *var1, longint_t *var2) {
    /*====================================================*/
    
    int numIntVar2 = 0;
    int numIntVar1 = 0;
    /* counting number of digiits in var1 and var2 */
    for (int i = 1; var2[i] != -1; i++) { numIntVar2++; }
    for (int i = 1; var1[i] != -1; i++) { numIntVar1++; }
    int intdiff = 0;
    
    /* calculating the difference in case var1 has less numbers than var2 */
    intdiff = numIntVar2 - numIntVar1;
    
    /* Given var2 has more digits, fill var1 with zeros to avoid weird addition */
    if (intdiff > 0) {
        for (int i = numIntVar1+1; i <= numIntVar2; i++) {
            var1[i] = 0;
        }
    }
    
    /*====================================================*/
    
	int newcomp = 0;
	
	// looping through var1 and var2
	for (int i = 1; i < INTSIZE+1; i++) {
	    //checking if var1 and var2 have valid digits (so we don't use -1 values)
	    if (var1[i] >= 0 && var2[i] >= 0) {
	        //checking if sum of values is greater than 10
	        if (var1[i] + var2[i] >= 10) {
	            // sum of values is greater than 10
	            
	            // store sum of values
	            newcomp = var1[i] + var2[i];
                
                /* splitValues array given a value of 2 because that's the max
                amount of digits that can be formed */
                int splitValues[2];
                int digit_count;
                
                /* We need to split the integer because we can't have a two digit num
                   occupying a single digit in an array */
                splitInteger(newcomp, splitValues, &digit_count);

                /* check if next digit is 9 (can't add) */
                if (var1[i+1] == 9) {
                    var1[i] = splitValues[1];
                    var2[i+1] += 1;
                } else {
                    /* Check if the next digit in var1 is not empty (-1) */
                    if (var1[i+1] != -1) {
                        /* adding the carry */
                        var1[i+1] += splitValues[0];
                        /* set to the least significant figure */
                        var1[i] = splitValues[1];
                    } else {
                        /*If the next digit in var1 is empty (-1),
                        increment next digit by 2 to accomodate for carry*/
                        var1[i] = splitValues[1];
                        var1[i+1] += 2;
                    }
                }
	        } else {
	            // if sum of values is less than 10 then just add
	            var1[i] += var2[i];
	        }
	    }
	}
}

/*****************************************************************
******************************************************************

Put your new functions below this line. Make sure you add suitable
prototypes at the top of the program.

******************************************************************
*****************************************************************/

void do_mult(longint_t *var1, longint_t *var2) {
    /*====================================================*/

    int numIntVar2 = 0;
    int numIntVar1 = 0;
    /* counting number of digiits in var1 and var2 */
    for (int i = 1; var2[i] != -1; i++) { numIntVar2++; }
    for (int i = 1; var1[i] != -1; i++) { numIntVar1++; }

    /*====================================================*/

    /* calculate the length of the result */
    int lengthNumber = numIntVar1 + numIntVar2 - 1;
    longint_t result[lengthNumber];

    /* Initialize result to zero */
    for (int i = 0; i <= lengthNumber; i++) {
        result[i] = 0;
    }

    for (int i = 1; i <= numIntVar1; i++) {
        int carry = 0;
        for (int j = 1; j <= numIntVar2; j++) {
            /* determining the product of the two values */
            int product = var1[i] * var2[j] + carry + result[i + j - 1];
            /* storing  last digit of the product in result */
            result[i + j - 1] = product % 10;
            /* upadting carry for next iteration */
            carry = product / 10;
        }
        /* add remaining carry for next position */
        result[i + numIntVar2] += carry;
    }

    /* Update the length of the result */
    while (lengthNumber > 1 && result[lengthNumber] == 0) {
        lengthNumber--;
    }
    result[0] = lengthNumber;

    /* Copy the result back to var1 */
    for (int i = 1; i <= lengthNumber; i++) {
        var1[i] = result[i];
    }
}

/* algorithms are fun! */