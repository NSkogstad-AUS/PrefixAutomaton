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
                                        -en (access get_or_create_new_state) */

/* SELF MADE FUNCTIONS ------------------------------------------------------------*/
automaton_t* create_automaton() {
    // Initialising a automaton we can work with //
    automaton_t* automaton = malloc(sizeof(automaton_t));

    // Initialising the first sate within the automaton //
    state_t* initial_state = malloc(sizeof(state_t));

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
    node_t* cursor = current->outputs ? current->outputs->head : NULL;

    // Iterates through the linked list of transitions associated with 'current'  //
    while (cursor) {
        // Checks if transition node matches input character
        if (cursor->str[0] == ch) { 
            /* It does, so transition with this state already exists, so we return
            the state the transition points to                                    */
            cursor->state->freq += 1;
            return cursor->state;
        }
        // It doesn't so we move to next transition node in the list //
        cursor = cursor->next;
    }
    

    /* If there is no output from the while loop, there is no exisiting transition
    so now we need to allocate new state and node for that transition             */


    // Initialising the new state within the automaton //
    state_t* new_state = malloc(sizeof(state_t));

    // Setting the new state's values //
    new_state->id = current->id + 1;
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
    
    // Accessing a loop which goes through the input and analyses each character //
    for (size_t i = 0; i < strlen(input); i++) {
        char ch = input[i];
        current_state = get_or_create_next_state(current_state, ch, automaton);
    }
}

void print_transitions(list_t* list) {
    if (!list) return;

    node_t* cursor = list->head;
    while (cursor) {
        printf("Transition on '%s' to State %u\n", cursor->str, cursor->state->id);
        cursor = cursor->next;
    }
}

void print_state(state_t* state) {
    if (!state || state->visited) return;

    printf("State %u (freq: %u):\n", state->id, state->freq);
    print_transitions(state->outputs);

    // Mark the state as visited to avoid infinite loops in case of cyclic references
    state->visited = 1;

    // Recursively print the output states
    if (state->outputs) {
        node_t* cursor = state->outputs->head;
        while (cursor) {
            print_state(cursor->state);
            cursor = cursor->next;
        }
    }
}

void print_automaton(automaton_t* automaton) {
    if (!automaton || !automaton->ini) {
        printf("Empty automaton.\n");
        return;
    }

    printf("Automaton starting from State %u:\n", automaton->ini->id);
    print_state(automaton->ini);

    // Reset the visited flags for next use (optional, but good practice)
    // state_reset_visited(automaton); // Commented as the function implementation was not provided
}

/* WHERE IT ALL HAPPENS -----------------------------------------------------------*/
int main(int argc, char *argv[]) {
    // Message from Artem: The proposed in this skeleton file #define's,
    // typedef's, and struct's are the subsets of those from my sample solution
    // to this assignment. You can decide to use them in your program, or if
    // you find them confusing, you can remove them and implement your solution
    // from scratch. I will share my sample solution with you at the end of
    // the subject.

    // We create an automaton and initalise it with an initial state //
    automaton_t* automaton = create_automaton();

    // Now that we have this initial automaton, we want to process new inputs //
    process_input(automaton, "Hey#Prof");
    process_input(automaton, "Hi#Sir");
    process_input(automaton, "Hi#Sir");
    process_input(automaton, "Hi#Prof");
    process_input(automaton, "Hello");
    process_input(automaton, "Hey");
    process_input(automaton, "Hi#there");
    process_input(automaton, "Hey#you");
    process_input(automaton, "");
    process_input(automaton, "Hey#");
    process_input(automaton, "Hi#T");
    process_input(automaton, "Hey");
    process_input(automaton, "Hel");
    process_input(automaton, "");
    process_input(automaton, "12");
    process_input(automaton, "Hi");
    process_input(automaton, "Hi#t");
    process_input(automaton, "He");

    print_automaton(automaton);

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
