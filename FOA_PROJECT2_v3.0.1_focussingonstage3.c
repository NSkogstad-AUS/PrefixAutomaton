#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S ----------------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n"   // stage delimiter
#define MDELIM "-------------------------------------\n"    // delimiter of -'s
#define THEEND "==THE END============================\n"    // end message
#define NOSFMT "Number of statements: %d\n"                 // no. of statements
#define NOCFMT "Number of characters: %d\n"                 // no. of chars
#define NPSFMT "Number of states: %d\n"                     // no. of states
#define TFQFMT "Total frequency: %d\n"                      // total frequency

#define CRTRNC '\r'                             // carriage return character
static int idcounter = 0;
static int charcounter = 0;
static int statementcounter = 0;
static int statecount = 0;

/* TYPE DEFINITIONS ---------------------------------------------------------------*/
typedef struct state state_t;   // a state in an automaton
typedef struct node  node_t;    // a node in a linked list

struct node {                   // a node in a linked list of transitions has
    char*           str;        // ... a transition string
    state_t*        state;      // ... the state reached via the string, and
    node_t*         next;       // ... a link to the next node in the list.
};

typedef struct {                // a linked list consists of
    node_t*         head;       // ... a pointer to the first node and
    node_t*         tail;       // ... a pointer to the last node in the list.
} list_t;

struct state {                  // a state in an automaton is characterized by
    unsigned int    id;         // ... an identifier,
    unsigned int    freq;       // ... frequency of traversal,
    int             visited;    // ... visited status flag, and
    list_t*         outputs;    // ... a list of output states.
};

typedef struct {                // an automaton consists of
    state_t*        ini;        // ... the initial state, and
    unsigned int    nid;        // ... the identifier of the next new state.
} automaton_t;

/* USEFUL FUNCTIONS ---------------------------------------------------------------*/
int mygetchar(void);                    // getchar() that skips carriage returns

automaton_t* create_automaton();        /* creating the automaton and returning 
                                        address of a struct type 'automaton_t'*/ 
                                        
state_t* get_or_create_next_state(state_t* current, char ch, automaton_t* automaton);
                                        /* identifying the next state or if not
                                        we will create a new state            */

void process_input(automaton_t* automaton, const char* input); 
                                        /* processing the input we've been giv-
                                        -en (access get_or_create_new_state)  */

void print_stage_zero();                // Printing out stage zero as required

void replay_phase(automaton_t* automaton, const char* input);
                                        /* Basically going through the replay of
                                        the entire automaton given noput      */

void generate_phase(state_t* latestaccessed, char* arrayrequired);
                                        /* Generating the most likely outcome g-
                                        -iven an input (Hel..lo)              */

/* SELF MADE FUNCTIONS ------------------------------------------------------------*/
automaton_t* create_automaton() {
    // Initialising a automaton we can work with //
    automaton_t* automaton = malloc(sizeof(automaton_t));

    // Initialising the first sate within the automaton //
    state_t* initial_state = malloc(sizeof(state_t));

    // Initialised a new state so increase state count //
    statecount += 1;

    // Setting the initial state's values //
    initial_state->id = 0;
    initial_state->freq = 0;
    initial_state->visited = 0;
    initial_state->outputs = NULL;

    // Creating an automaton with initial_state as it's first state //
    automaton->ini = initial_state;
    automaton->nid = 1;

    // Returning a succesffuly constructed automaton //
    return automaton;
}

state_t* get_or_create_next_state(state_t* current, char ch, automaton_t* automaton) {
    /* Checking if the list of outputs associated with the current state is empty or
    not, if isn't, cursor will point to the first node in the list of transitions */
    node_t* cursor; //= current->outputs ? current->outputs->head : NULL;
    if (current -> outputs) {
        cursor = current->outputs->head;
    } else {
        cursor = NULL;
    }

    // Iterates through the linked list of transitions associated with 'current'  //
    while (cursor) {
        // Checks if transition node matches input character
        if (cursor->str[0] == ch) { 
            /* It does, so transition with this state already exists, so we return
            the state the transition points to                                    */
            charcounter += 1;
            cursor->state->freq += 1;
            return cursor->state;
        }
        // It doesn't so we move to next transition node in the list //
        cursor = cursor->next;
    }
    // adding up to global varaible that we can reference when printing stage 0 //
    charcounter += 1;
    statecount += 1;
    

    /* If there is no output from the while loop, there is no exisiting transition
    so now we need to allocate new state and node for that transition             */


    // Initialising the new state within the automaton //
    state_t* new_state = malloc(sizeof(state_t));

    // Setting the new state's values //
    new_state->id = idcounter + 1;
    idcounter += 1;
    new_state->freq = 1;
    new_state->visited = 0;
    new_state->outputs = NULL;

    // Initialising the new node within the automaton //
    node_t* new_node = malloc(sizeof(node_t));

    // Setting the new node's values //
    new_node->str = malloc(2);
    new_node->str[0] = ch;
    new_node->str[1] = '\0';
    new_node->state = new_state;
    new_node->next = NULL;

    
    // Checking if the current state has outputs //
    if (!current->outputs) {
        /* If the current state has no outputs, allocate new memory and initalise
        to the new transition node */
        current->outputs = malloc(sizeof(list_t));
        current->outputs->head = current->outputs->tail = new_node;
    } else {
        /* If current state has no output transitions, append new transition node 
        to the end of the list and update the tail                                */
        current->outputs->tail->next = new_node;
        current->outputs->tail = new_node;
    } 

    return new_state;
}

void process_input(automaton_t* automaton, const char* input) {
    // Setting the current_state we've accessed //
    state_t* current_state = automaton->ini;

    // Keeping track of how many statements processed //
    if (strlen(input) > 0) {
        statementcounter += 1;
    }
    
    // Accessing a loop which goes through the input and analyses each character //
    for (size_t i = 0; i < strlen(input); i++) {
        char ch = input[i];
        current_state = get_or_create_next_state(current_state, ch, automaton);
    }

    // Essentially saying 'ok if it's at the end, set the freq to zero' //
    if (current_state) {
        current_state -> freq = 0;
    }
}

void print_stage_zero() {
    // Printing all the counted statements (num of input lines) //
    printf("statementcount: %d\n", statementcounter);
    // Printing all the counted characters //
    printf("charcount: %d\n", charcounter);
    // Printing the number of statements accessed //
    printf("statecount: %d\n", statecount);
}

void replay_phase(automaton_t* automaton, const char* input) {
    // Setting initial variable and key points of reference //
    state_t* current_state = automaton->ini;

    int input_length = strlen(input);
    int arrSize = input_length + 4;
    int capitalchange = 0;
    int idx = 0;

    char *arr = malloc(arrSize * sizeof(char));

    // Beginning the replay of the input //
    for(int i = 0; i < input_length; i++) {
        /* Initialising a 'cursor' which points to the first o
        utput of the latest accessed state                  */
        node_t* cursor = current_state->outputs->head;

        // Found the same value initially set to false //
        int found = 0;

        // While the outputs list still has nodes left to access // 
        while (cursor) {
            // Checking node if node str is equal to the char of the input //
            if (cursor->str[0] == input[i] || cursor->str[0] == (input[i] + 'a' - 'A')) {
                // If it is, append it to a new array //
                arr[idx++] = input[i];
                // Current accessed state is updated //
                current_state = cursor->state;
                found = 1;
                // Checking if char in input has a cooresponding capital in cursor //
                if (cursor->str[0] == (input[i] + 'a' - 'A')) {
                    capitalchange = 1;
                }
                break;
            }
            // Accessing the next node in the outputs list //
            cursor = cursor->next;
        }
    }

    // Adding the three dots at the end //
    arr[idx++] = '.';
    arr[idx++] = '.';
    arr[idx++] = '.';
    arr[idx++] = '\0';

    // Checking if there is a cooresponding capital //
    if (capitalchange != 1) {
        // If not, generate the next part of the output //
        generate_phase(current_state, arr);
        capitalchange = 0;
    } else {
        // If there is, just print the old array //
        printf("%s\n", arr);
    }
}

void generate_phase(state_t* latestaccessed, char* arrayrequired) {
    // Setting initial variable and key points of reference //
    node_t* cursor = latestaccessed->outputs->head;
    node_t* biggestnode = cursor;
    int max_freq = 0;
    char *new_array = NULL;

    // While the outputs list still has nodes left to access // 
    while (cursor) {
        // If the current state has a higher frequency than max, update //
        if (cursor->state->freq > max_freq) {
            max_freq = cursor->state->freq;
            // keeping track of the biggestnode //
            biggestnode = cursor;
        }
        // Checking if the current state has an equal frequency//
        if (cursor->state->freq == max_freq) {
            // If it does, we pick lowest ASCII char as priority //
            while (cursor->next) {
                node_t* nextcursor = cursor->next;
                // If first string is greater than the next, update //
                if (cursor->str > nextcursor->str) {
                    biggestnode = cursor;
                    max_freq = cursor->state->freq;
                    break;
                } 
                // If second string is greater than the next, update //
                else {
                    max_freq = nextcursor->state->freq;
                    biggestnode = nextcursor;
                    break;
                }
            }
        }
        // Accessing the next node in the outputs list //
        cursor = cursor->next;    
    }

    // If we have found a new node with the highest frequency, access //
    if (biggestnode) {
        // Reallocating the array and updating its input values //
        int i = strlen(arrayrequired);
        arrayrequired = realloc(arrayrequired, (i+2) * sizeof(char));
        arrayrequired[i] = *(biggestnode->str);
        arrayrequired[i+1] = '\0';

        // Checking if the max_freq has reached the end of a statement //
        if (max_freq != 0) {
            /* If not, we want to continue accessing the rest of the states
            within the statement linked list                             */
            generate_phase(biggestnode->state, arrayrequired);
        } else {
            // If we have, then print what the array we've updated //
            printf("%s\n", arrayrequired);
        }
    }
}

void state_compression(state_t* state, state_t* nextstate) {
    
}

/* WHERE IT ALL HAPPENS -----------------------------------------------------------*/
int main(int argc, char *argv[]) {
    // Message from Artem: The proposed in this skeleton file #define's,
    // typedef's, and struct's are the subsets of those from my sample solution
    // to this assignment. You can decide to use them in your program, or if
    // you find them confusing, you can remove them and implement your solution
    // from scratch. I will share my sample solution with you at the end of
    // the subject.
    // Opening the files //

    // We create an automaton and initalise it with an initial state //
    automaton_t* automaton = create_automaton();

    // Opening a new file containing the values we want to access //
    FILE *file = fopen("/Users/nicolaiskogstad/Documents/FOA/ASSIGNMENT2/inputfile.txt", "r");

    // If there is no file, we have a problem //
    if (!file) {
        printf("file was not able to be read\n");
        // exit the program //
        exit(1);
    }

    // Setting initial variable and key points of reference //
    char line[10];
    int firststagereached = 1;
    int secondstagereached = 0;
    int thirdstagereached = 0;
    int i = 0;

    // While the input file has not had its end reached //
    while (fgets(line, sizeof(line), file) != NULL) {
        /* Removing the newline char so fgets doesn't include it
        in returned stirng                                    */
        char *p = line;
        while (*p) {
            if (*p == '\n') {
                // Replace it with null to say string ends here //
                *p = '\0';
                break;
            }
            p++;
        }

        // Whilst the first stage is still being processed //
        if (firststagereached == 1) {
            // Process the lines //
            process_input(automaton, line);
        }
        
        // We have reached the first blank line input, so we update to next stage //
        if (strlen(line) == 0) {
            // Second stage has not been reached yet so we access it first before third //
            if (secondstagereached == 0) {
                // Print the divider and stage zero output //
                printf(SDELIM, 1);
                print_stage_zero();
                printf(SDELIM, 2);
                // Update that we are at the next stage //
                secondstagereached = 1;
                firststagereached = 0;
                // Continue out of the if //
                continue;
            }
            // third stage has not been reached yet so we access it after the second //
            if (thirdstagereached == 0) {
                // Print the divider //
                printf(SDELIM, 3);
                // Update that we are at the next stage //
                secondstagereached = 0;
                thirdstagereached = 1;
                // Continue out of the if //
                continue;
            }
        }

        // Second stage has been reached, access it //
        if (secondstagereached == 1) {
            replay_phase(automaton, line);
        }

        // Third stage has been reached, access it //
        if (thirdstagereached == 1) {
        }
    }

    // Close the file, we have done what was needed //
    fclose(file);

    return EXIT_SUCCESS;        // algorithms are fun!!!
}

/* USEFUL FUNCTIONS ---------------------------------------------------------------*/

// An improved version of getchar(); skips carriage return characters.
// NB: Adapted version of the mygetchar() function by Alistair Moffat
int mygetchar() {
    int c;
    while ((c=getchar())==CRTRNC);
    return c;
}

/* THE END ------------------------------------------------------------------------*/
