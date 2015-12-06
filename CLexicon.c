/* Filename: CLexicon.c
 * Author: Dan McFalls (dmcfalls@stanford.edu)
 * -------------------------------------------
 * A Lexicon is a data structure used for the efficient storage and
 * retrieval of strings (char*). It functions the same as a set but is
 * internally optimized to process strings efficiently. The internal
 * structure of the CLexicon is a tree of "LexNodes" in which each LexNode
 * has 26 children, one for each letter of the English alphabet.
 *
 * Modeled after the "Lexicon" class from the Stanford C++ Libraries.
 */

#include "CLexicon.h"   //includes <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ALPHA_SIZE 26
#define MAX_WORD_LEN 45


            /* * * Struct Definitions * * */


struct LexNode {
    bool is_word;
    LexNode* children[ALPHA_SIZE];
};

struct CLexiconImplementation {
    LexNode* root;
    int wordcount;
};


            /* * * Local Helper Functions * * */


/* Function: create_node
 * -----------------------
 * Creates a new LexNode, initializes all of its children pointers to NULL,
 * and sets its word status to false. Returns a pointer.
 */ 
static inline LexNode* create_node() {
    LexNode* node  = malloc(sizeof(LexNode));
    //Loop overhead amortization to improve efficiency; create_node is called very often.
    for(int i = 0; i < ALPHA_SIZE - 2; i+= 4) {
        node->children[i] = NULL; node->children[i+1] = NULL; node->children[i+2] = NULL; node->children[i+3] = NULL;
    }
    //Finishes the loop's job of initializing the node's child nodes.
    node->children[ALPHA_SIZE - 2] = NULL; node->children[ALPHA_SIZE - 1] = NULL;
    node->is_word = false;
    return node;
}

/* Function: delete_node_helper
 * ----------------------------
 * Helper function for delete_node. Passes LexNode pointers-to-pointers so that
 * the actual tree can be recursively deleted. Edits the lex's wordcount.
 */ 
static void delete_node_helper(CLexicon* lex, LexNode** nodeptr) {
    if(*nodeptr == NULL) return;

    //Loop overhead amortization to improve efficiency; delete_node_helper is called very often.
    for(int i = 0; i < ALPHA_SIZE; i+= 2) {
        delete_node_helper(lex, &(*nodeptr)->children[i]);   delete_node_helper(lex, &(*nodeptr)->children[i+1]);
    }
    if((*nodeptr)->is_word) lex->wordcount--;
    free(*nodeptr);
    *nodeptr = NULL;
}

/* Function: delete_node
 * ---------------------
 * Deletes a node and all of its children nodes and frees all memory associated with them.
 */ 
static inline void delete_node(CLexicon* lex, LexNode* node) {
    if(node == NULL) return;

    for(int i = 0; i < ALPHA_SIZE; i++) {
        delete_node_helper(lex, &node->children[i]);
    }
    free(node);
    node = NULL;
}

/* Function: branch_contains_words
 * -------------------------------
 * Returns true if node at nodeptr represents a word or if any of its children contain a branch with
 * a node representing a word. Otherwise, returns false.
 */ 
static bool branch_contains_words(CLexicon* lex, LexNode** nodeptr) {
    if(*nodeptr == NULL) return false;

    bool words_in_branch = false;
    for(int i = 0; i < ALPHA_SIZE; i++) {
        words_in_branch = words_in_branch || branch_contains_words(lex, &(*nodeptr)->children[i]);
    }

    return words_in_branch || (*nodeptr)->is_word;
}

/* Function: clean_empty_branches
 * ------------------------------
 * Removes all nodes from the tree beginning at node that have no children representing words.
 * Effectively "cleans up" branches of the tree that are no longer in use. Called during
 * the remove function to ensure the contains_prefix functionality is as-intended.
 */ 
static void scrub_empty_branches(CLexicon* lex, LexNode** nodeptr) {
    //If the node at nodeptr is NULL, does nothing.
    if(*nodeptr == NULL) return;

    bool words_in_branch = branch_contains_words(lex, nodeptr);
    //If there are words in this branch, go down a level and check for empty branches in children.
    if((*nodeptr)->is_word || words_in_branch) {
        for(int i = 0; i < ALPHA_SIZE; i++) {
            scrub_empty_branches(lex, &(*nodeptr)->children[i]);
        }
    //Else, delete all of node's children and then delete node.
    } else {
        delete_node(lex, *nodeptr);
        *nodeptr = NULL;
    }
}

/* Function: to_lower_case
 * -----------------------
 * Creates a lower-case copy of word with length wordlen and stores it in word_lower.
 */ 
static inline void to_lower_case(char word_lower[], int wordlen, char* word) {
    for(int i = 0; i < wordlen; i++) {
        word_lower[i] = tolower(word[i]);
    }
    word_lower[wordlen] = '\0';
}

/* Fuction: clex_simple_add
 * ------------------------
 * Adds a word in all lower-case with length wordlen to the lexicon. Avoids checking the
 * case of the word. Retains the "clex" prefix because it could theoretically be listed
 * in the header file and considered available for client use. Remains hidden now for the
 * sake of simplicity of presentation.
 */ 
static inline void clex_simple_add(CLexicon* lex, char* word_lower, int wordlen) {
    //For every character in the word, accesses (or creates) subnodes.
    LexNode* last_node = lex->root;
    for(int i = 0; i < wordlen; i++) {
        char ith = word_lower[i];
        LexNode** next_node = &last_node->children[ith - 'a'];
        if(*next_node == NULL) {
            *next_node = create_node();
        }
        last_node = *next_node;
    }
    //Indicates that the final node, representing the string word, is added to the lexicon.
    last_node->is_word = true;
    lex->wordcount++;
}

/* Function: clex_contains_helper
 * ------------------------------
 * Helper method for contains that takes as an argument whether to search for prefix or word.
 */ 
static inline bool clex_contains_helper(CLexicon* lex, char* word, bool isPrefix) {
    //Creates a lower-case copy of the word.
    int wordlen = strlen(word);
    char word_lower[wordlen+1];
    to_lower_case(word_lower, wordlen, word);

    //Traverses the tree from the root until it reaches the target node.
    LexNode* curr_node = lex->root;
    for(int i = 0; i < wordlen; i++) {
        //If the tree ends before target node is reached, simply returns false.
        if(curr_node == NULL) return false;
        char ith = word_lower[i];
        curr_node = curr_node->children[ith - 'a'];
    }
    if(curr_node == NULL) return false;

    //Returns true if searching for a prefix, otherwise returns the node's is_word field.
    return isPrefix || curr_node->is_word;
}


            /* * * Client Functions Listed in the Header File * * */


/* Function: clex_create
 * ---------------------
 * Creates a new CLexicon* which is initialized and returned. The client only accesses
 * a pointer to the CLexicon and should never need to interact with the actual struct.
 */
CLexicon* clex_create() {
    CLexicon* lex = malloc(sizeof(CLexicon));
    lex->root = create_node();
    lex->wordcount = 0;
    return lex;
}

/* Function: clex_delete
 * ---------------------
 * Frees all memory associated with a CLexicon*.
 */ 
void clex_delete(CLexicon* lex) {
   delete_node(lex, lex->root);
   free(lex);
}

/* Function: clex_add
 * ------------------
 * Adds the given word to the CLexicon after converting it to lower case.
 */ 
void clex_add(CLexicon* lex, char* word) {
    //Creates a lower-case copy of the word.
    int wordlen = strlen(word);
    char word_lower[wordlen+1];
    to_lower_case(word_lower, wordlen, word);
    
    //simple_add is a helper function that adds the word and increments the wordcount.
    clex_simple_add(lex, word_lower, wordlen);
}

/* Function: clex_add_from_file
 * ----------------------------
 * This function fills a lexicon with entires from a text file where each line
 * contains a single string of text with letters a-z or A-Z. If the file cannot
 * be opened or a bad line of text is reached, the function returns false to indicate
 * a failure in reading. If "is_lower_case" is true, uses clex_simple_add to add each
 * line, which does not check case. If it is false, uses clex_add, which is more expensive.
 */ 
bool clex_add_from_file(CLexicon* lex, char* filename, bool is_lower_case) {
    FILE* lex_file = fopen(filename, "r");
    if(lex_file == NULL) {
        return false;
    }

    char line[MAX_WORD_LEN+1];
    //Reads lines from the file, adding them to the lexicon one by one.
    while(fgets(line, sizeof(line), lex_file)) {
        //Scans the line to make sure that it was processed correctly. 45 is MAX_WORD_LEN.
        char word[MAX_WORD_LEN+1];
        int nscanned = sscanf(line, "%45[ a-zA-Z]", word);
        if(nscanned != 1) {
            fclose(lex_file);
            return false;
        }

        //Adds the word depending on whether it is guarunteed to be lower case or not.
        is_lower_case ? clex_simple_add(lex, word, strlen(word)) : clex_add(lex, word);
    }

    fclose(lex_file);
    return true; 
}

/* Function: clex_clear
 * --------------------
 * Deletes all entires from the CLexicon, initializes a new root node, and sets
 * the word count to zero.
 */ 
void clex_clear(CLexicon* lex) {
    delete_node(lex, lex->root);
    lex->root = create_node();
    lex->wordcount = 0;
}

/* Function: clex_contains
 * -----------------------
 * Traverses the tree from the root until it reaches the node that corresponds
 * to the given word. Returns that node's is_word field, indicating whether word
 * is a member of the CLexicon.
 */ 
bool clex_contains(CLexicon* lex, char* word) {
    //Calls the helper function with the "isPrefix" field set to false.
    return clex_contains_helper(lex, word, false);
}

/* Function: clex_contains_prefix
 * ------------------------------
 * Returns true if the lexicon contains any intermediate nodes corresponding to the given
 * prefix, regardless of whether their is_word field is true. Returns false if a NULL node
 * is reached while searching for the prefix.
 */ 
bool clex_contains_prefix(CLexicon* lex, char* prefix) {
    //Calls the helper function with the "isPrefix" field set to true.
    return clex_contains_helper(lex, prefix, true);
}

/* Function: clex_isEmpty
 * ----------------------
 * Returns true if the CLexicon is empty, false otherwise.
 * Named in lowerCamelCase because every isEmpty function I've ever encountered
 * has looked that way, and it just seems to fit better than is_empty.
 */ 
bool clex_isEmpty(CLexicon* lex) {
    return lex->wordcount == 0;
}

/* Function: clex_remove
 * ---------------------
 * Removes the target word from the CLexicon by traversing the tree until it reaches
 * the node corresponding to word, then changing its is_word field to false. If there
 * are no nodes beneath the word node, deletes the node from the tree and sets its
 * parent's pointer to it to NULL. Returns false if the word is not a member of the CLexicon.
 */ 
bool clex_remove(CLexicon* lex, char* word) {
    //Creates a lower-case copy of word.
    int wordlen = strlen(word);
    char word_lower[wordlen+1];
    to_lower_case(word_lower, wordlen, word);

    //Iterates through the letters, descending down the tree to the penultimate letter.
    LexNode** curr_node = &lex->root;
    for(int i = 0; i < wordlen - 1; i++) {
        if(*curr_node == NULL) return false;
        char ith = word_lower[i];
        curr_node = &((*curr_node)->children[ith - 'a']);
    }
    LexNode** word_node = &(*curr_node)->children[word_lower[wordlen - 1] - 'a'];
    if(*word_node == NULL) return false;
    (*word_node)->is_word = false;
    lex->wordcount--;

    //Checks to see if word_node has children. If not, deletes the node and points word_node to NULL.
    for(int i = 0; i < ALPHA_SIZE; i++) {
        if((*word_node)->children[i] != NULL) return true;
    }
    free(*word_node);
    *word_node = NULL;
    //Ensures that prefix functionality is maintained correctly by removing unused branches from tree.
    scrub_empty_branches(lex, &lex->root);
    return true;
}

/* Function: clex_remove_prefix
 * ----------------------------
 * Removes all words with the given prefix from the lexicon by traversing the tree to get to
 * the prefix node (if it exists) and then recursively descending down each of its child trees,
 * removing all nodes below it (uses the "delete_node_helper" method). Returns true if the
 * prefix and its subtree has been removed, false if the tree never contained the prefix.
 */
bool clex_remove_prefix(CLexicon* lex, char* prefix) {
    int preflen = strlen(prefix);
    char pref_lower[preflen+1];
    to_lower_case(pref_lower, preflen, prefix);

    LexNode** curr_node = &lex->root;
    for(int i = 0; i < preflen; i++) {
        if(*curr_node == NULL) return false;
        char ith = pref_lower[i];
        curr_node = &((*curr_node)->children[ith - 'a']);
    }
    if(*curr_node == NULL) return false;
    //Deletes the prefix node and its entire subtree.
    delete_node_helper(lex, curr_node);
    *curr_node = NULL;
    return true;
}

/* Function: clex_wordcount
 * ------------------------
 * Returns the number of entries in the CLexicon.
 */ 
int clex_wordcount(CLexicon* lex) {
    return lex->wordcount;
}
