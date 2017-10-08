# CLexicon
Author: Dan McFalls (dmcfalls@stanford.edu)

Implementation of the <code>lexicon</code> data type in C using a prefix-tree.

Uses a tree structure in which each parent node has 26 children nodes, each representing a letter of the English alphabet.

Equivalent to a <code>set</code> of strings, but optimized for significantly improved efficiency when searching a large set of words.

Primary functions are <code>add</code>, <code>contains</code>, and <code>remove</code>. Has function for adding words from a text file. Also has functionaltiy for seeing whether any words beginning with a prefix appear in the lexicon and for removing all words in the lexicon that have a given prefix.

CLexicon.h and CLexicon.c have the project code, and lextest.c is a simple client test program.
