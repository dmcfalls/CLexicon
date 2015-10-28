/* File: lextest.c
 * Author: Dan McFalls (dmcfalls@stanford.edu)
 * ---------------
 * Simple client program for testing the features of CLexicon.
 */ 

#include "CLexicon.h"
#include "CLexicon.c"
#include <stdbool.h>

void simple_test() {
    printf("---------- Running Simple Test ----------\n");
    
    CLexicon* lex = clex_create();

    printf("adding items to lexicon\n\n");

    clex_add(lex, "hello");
    clex_add(lex, "apple");
    clex_add(lex, "pear");

    printf("contains 'hello'? (expect true) : %s\n", clex_contains(lex, "hello") ? "true" : "false");
    printf("contains 'apple'? (expect true) : %s\n", clex_contains(lex, "apple") ? "true" : "false");
    printf("contains 'pear'? (expect true) : %s\n", clex_contains(lex, "pear") ? "true" : "false");
    printf("contains 'notaword'? (expect false) : %s\n", clex_contains(lex, "notaword") ? "true" : "false");
    printf("contains 'hel'? (expect false) : %s\n\n", clex_contains(lex, "hel") ? "true" : "false");

    printf("lexicon has %d elements (expect 3)\n\n", clex_wordcount(lex));

    printf("removing items from lexicon\n\n");

    clex_remove(lex, "hello");
    clex_remove(lex, "apple");
    clex_remove(lex, "pear");
    
    printf("lexicon has %d elements (expect 0)\n\n", clex_wordcount(lex));

    printf("contains 'hello'? (expect false) : %s\n", clex_contains(lex, "hello") ? "true" : "false");
    printf("contains 'apple'? (expect false) : %s\n", clex_contains(lex, "apple") ? "true" : "false");
    printf("contains 'pear'? (expect false) : %s\n", clex_contains(lex, "pear") ? "true" : "false");

    clex_delete(lex);
    printf("\n");
}

void file_reading_test() {
    printf("---------- Running File Reading Test ----------\n");

    char* filename = "dictionary.txt";
    CLexicon* lex = clex_create();

    printf("adding words from file...\n");
    bool successful = clex_add_from_file(lex, filename, true);

    printf("was file add successful? (expect true) %s\n", successful ? "true" : "false");
    printf("word count: %d\n\n", clex_wordcount(lex));

    printf("contains 'hello'? (expect true) : %s\n", clex_contains(lex, "hello") ? "true" : "false");
    printf("contains 'apple'? (expect true) : %s\n", clex_contains(lex, "apple") ? "true" : "false");
    printf("contains 'pear'? (expect true) : %s\n", clex_contains(lex, "pear") ? "true" : "false");
    printf("contains 'notaword'? (expect false) : %s\n\n", clex_contains(lex, "notaword") ? "true" : "false");

    printf("contains prefix 'sub'? (expect true) : %s\n", clex_contains_prefix(lex, "sub") ? "true" : "false");
    printf("contains prefix 'incre'? (expect true) : %s\n", clex_contains_prefix(lex, "incre") ? "true" : "false");
    printf("contains prefix 'flupsz'? (expect false) : %s\n\n", clex_contains_prefix(lex, "flupsz") ? "true" : "false");

    printf("clearing lexicon...\n");
    clex_clear(lex);
    printf("word count (expect 0): %d\n", clex_wordcount(lex));

    printf("deleting lexicon\n");
    clex_delete(lex);

    printf("done!\n\n");
}

int main(int argc, char *argv[]) {
    simple_test();
    file_reading_test();
    return 0;
}
