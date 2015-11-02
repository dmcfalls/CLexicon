# CLexicon
Author: Dan McFalls (dmcfalls@stanford.edu)

Implementation of the lexicon data type in C using a prefix-tree.

Uses a tree structure in which each parent node has 26 children nodes, each representing a letter of the English alphabet.

Equivalent to a set of strings, but optimized for significantly improved efficiency when searching a large set of words.

Primary functions are add, contains, and remove. Has function for adding words from a text file. Also has functionaltiy for seeing whether any words beginning with a prefix appear in the lexicon and removing all words in the lexicon that have a given prefix.

CLexicon.h and CLexicon.c have the project code, and lextest.c is a simple client test program.
