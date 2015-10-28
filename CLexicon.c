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

#include "CLexicon.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ALPHA_SIZE 26
#define MAX_WORD_LEN 45


            /*** struct definitions ***/


struct LexNode {
    bool is_word;
    LexNode* children[ALPHA_SIZE];
};

struct CLexiconImplementation {
    LexNode* root;
    int wordcount;
};


            /*** local helper functions ***/


/* Function: create_node
 * -----------------------
 * Creates a new LexNode, initializes all of its children pointers to NULL,
 * and sets its word status to false. Returns a pointer.
 */ 
LexNode* create_node() {
    LexNode* node  = malloc(sizeof(LexNode*) + sizeof(LexNode));
    for(int i = 0; i < ALPHA_SIZE; i++) {
        node->children[i] = NULL;
    }
    node->is_word = false;
    return node;
}

/* Function: delete_node_helper
 * ----------------------------
 * Helper function for delete_node. Passes LexNode pointers-to-pointers so that
 * the actual tree can be recursively deleted.
 */ 
void delete_node_helper(LexNode** nodeptr) {
    if(*nodeptr == NULL) return;

    for(int i = 0; i < ALPHA_SIZE; i++) {
        delete_node_helper(&(*nodeptr)->children[i]);
    }
    free(*nodeptr);
}

/* Function: delete_node
 * ---------------------
 * Deletes a node and all of its children nodes and frees all memory associated with them.
 */ 
void delete_node(LexNode* node) {
    if(node == NULL) return;

    for(int i = 0; i < ALPHA_SIZE; i++) {
        delete_node_helper(&node->children[i]);
    }
    free(node);
}

/* Fuction: clex_simple_add
 * ------------------------
 * Adds a word in all lower-case with length wordlen to the lexicon. Avoids checking the
 * case of the word. Retains the "clex" prefix because it could theoretically be listed
 * in the header file and considered available for client use. Remains hidden now for the
 * sake of simplicity of presentation.
 */ 
void clex_simple_add(CLexicon* lex, char* word_lower, int wordlen) {
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

bool clex_contains_helper(CLexicon* lex, char* word, bool isPrefix) {
    //Creates a lower-case copy of the word.
    int wordlen = strlen(word);
    char word_lower[wordlen+1];
    for(int i = 0; i < wordlen; i++) {
        word_lower[i] = tolower(word[i]);
    }
    word_lower[wordlen] = '\0';

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
    if(isPrefix) return true;
    return curr_node->is_word;
}


            /*** client functions listed in the header file ***/


/* Function: clex_create
 * ---------------------
 * Creates a new CLexicon* which is initialized and returned. The client only accesses
 * a pointer to the CLexicon and should never need to interact with the actual struct.
 */
CLexicon* clex_create() {
    CLexicon* lex = malloc(sizeof(CLexicon*) + sizeof(CLexicon));
    lex->root = create_node();
    lex->wordcount = 0;
    return lex;
}

/* Function: clex_delete
 * ---------------------
 * Frees all memory associated with a CLexicon*.
 */ 
void clex_delete(CLexicon* lex) {
   delete_node(lex->root);
   free(lex);
}

/* Function: clex_add
 * ------------------
 * Adds the given word to the CLexicon after converting it to lower case.
 */ 
void clex_add(CLexicon* lex, char* word) {
    //printf("adding word: %s\n", word);
    int wordlen = strlen(word);
    //Creates a lower-case copy of the word.
    char word_lower[wordlen+1];
    for(int i = 0; i < wordlen; i++) {
        word_lower[i] = tolower(word[i]);
    }
    word_lower[wordlen] = '\0';

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
    delete_node(lex->root);
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
    for(int i = 0; i < wordlen; i++) {
        word_lower[i] = tolower(word[i]);
    }
    word_lower[wordlen] = '\0';

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

    //Checks to see if word_node has children. If not, deletes the node and points curr_node to NULL.
    for(int i = 0; i < ALPHA_SIZE; i++) {
        if((*word_node)->children[i] != NULL) return true;
    }
    free(*word_node);
    *word_node = NULL;
    return true;
}

/* Function: clex_wordcount
 * ------------------------
 * Returns the number of entries in the CLexicon.
 */ 
int clex_wordcount(CLexicon* lex) {
    return lex->wordcount;
}
