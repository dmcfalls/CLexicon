/* File: CLexicon.h
 * Author: Dan McFalls (dmcfalls@stanford.edu)
 * -------------------------------------------
 * CLexicon is a data structure created for the efficient storage
 * and retrieval of words (strings). It is equivalent to a set but
 * is internally optimized to process strings very efficiently.
 *
 * Modeled after "Lexicon" from the Stanford C++ Libraries.
 */

#ifndef _CLexicon_h
#define _CLexicon_h

#include <stdbool.h>    //defines the bool type


            /*** struct partial definitions ***/


/* Struct: LexNode
 * ---------------
 * Incomplete declaration of struct LexNode. LexNode is the basic unit of the CLexicon,
 * and is used to create the tree structure with hightened efficiency for word processing.
 * Its specifications remain opaque in the header file.
 */ 
typedef struct LexNode LexNode;

/* Struct: CLexiconImplementation
 * ------------------------------
 * Incomplete declaration of struct CLexiconImplementation. The actual implementation of the
 * CLexicon occurs in the C file to keep the header file clean and straightforward.
 */ 
typedef struct CLexiconImplementation CLexicon;


            /*** "public" methods intended for client use ***/


/* Function: clex_create
 * ---------------------
 * Creates a pointer to a CLexicon struct and initializes all of its fields.
 * The client accesses only these external methods and does not worry about the
 * internal structure of the CLexicon. This prevents tampering that might cause
 * certain methods to stop functioning correctly.
 * Runs in constant time.
 */ 
CLexicon* clex_create();


/* Function: clex_delete
 * ---------------------
 * Deletes the CLexicon and frees all memory associated with it.
 * Runs in linear time (scaling with the size of the lexicon).
 */ 
void clex_delete(CLexicon* lex);


/* Function: clex_add
 * ------------------
 * Adds the given string word to the CLexicon. This operation begins (relatively) slowly
 * when the first words are being added and increases in efficiency as the lexicon grows larger.
 * This is possible because the lexicon is structured specifically to work with strings.
 * Runs in linear time (scaling with the length of word).
 */
void clex_add(CLexicon* lex, char* word);


/* Function: clex_add_from_file
 * ----------------------------
 * Adds words from a file with the given name to the CLexicon. The file must be formatted such that
 * a single word appears on each line. Words should not exceed the maxmimum length of the longest
 * English word included in a major dictionary (pneumonoultramicroscopicsilicovalcanoconiosis).
 * The final boolean argument indicates whether the words read from the file are guaranteed to be
 * in lower case. If this is true, the operation for adding each word will be more efficient.
 * Runs in quadratic time (scaling with file size and lengths of words).
 */ 
bool clex_add_from_file(CLexicon* lex, char* filename, bool is_lower_case);


/* Function: clex_clear
 * --------------------
 * Clears all elements from the CLexicon. Distinct from clex_delete in that it does not
 * free the memory associated with the data structure, and leaves it able to be used for further
 * adding, searching, and removing operations.
 */ 
void clex_clear(CLexicon* lex);


/* Function: clex_contains
 * -----------------------
 * Searches the CLexicon for a given word and returns true if it contains it, otherwise returning
 * false. The internal structure of the lexicon ensures that this operation is significantly faster
 * than running a binary search on a sorted set of strings.
 * Runs in linear time (scaling with the length of the word).
 */ 
bool clex_contains(CLexicon* lex, char* word);


/* Function: clex_contains_prefix
 * ------------------------------
 * Returns true if the given lexicon contains any words beginning with the given prefix, or
 * false otherwise.
 */ 
bool clex_contains_prefix(CLexicon* lex, char* prefix);


/* Function: clex_isEmpty
 * ----------------------
 * Returns true if the given CLexicon is empty, false otherwise.
 * Runs in constant time.
 */ 
bool clex_isEmpty(CLexicon* lex);


/* Function: clex_remove
 * ---------------------
 * Removes the given word from the CLexicon, returning true on a successful removal.
 * If the word is not found in the CLexicon, returns false.
 * Runs in linear time (scaling with the length of the word).
 */ 
bool clex_remove(CLexicon* lex, char* word);


/* Function: clex_remove_prefix
 * ----------------------------
 * Removes all words beginning with the given prefix from the CLexicon, returning true if the prefix
 * and all words beginning with the prefix were removed, false if the prefix was not found in the tree.
 * Runs in quadratic time (scaling with the number of words with the prefix and their lengths).
 */
bool clex_remove_prefix(CLexicon* lex, char* prefix);


/* Function: clex_word_count
 * -------------------------
 * Returns the number of elements currently stored in the CLexicon.
 * Runs in constant time.
 */ 
int clex_wordcount(CLexicon* lex);

#endif
