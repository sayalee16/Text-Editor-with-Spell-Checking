#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_WORD_LENGTH 50
#define MAX_SIZE 34003 // Represents the maximum size of the hash table- is a prime number and is chosen accordingly to have the ideal load factor 0.75
#define MAX_SUGGESTIONS 10 // Represents the maximum number of suggestions for misspelled words

// Represents a node in the dictionary's hash table
typedef struct hashnode {
    char word[MAX_WORD_LENGTH]; // The word stored in the node
    struct hashnode *next;      // Pointer to the next node in the linked list
    int levenshtein;            // Levenshtein distance to a given word
    int lcp;                    // Longest common prefix length with a given word
} hashnode;

// Represents the dictionary implemented as a hash table
typedef struct dictionary {
    hashnode *h[MAX_SIZE]; // Array of pointers to hash nodes
} dictionary;


// Represents the array of pointers pointing to the nodes in the Suggestions
hashnode *suggestions[MAX_SUGGESTIONS];
int suggestion_count = 0;

//Global Declaration of Dictionary
dictionary d;


// Initializes the dictionary by setting all hash table buckets to NULL
void init(dictionary *d);

// Computes a hash value for a given word using polynomial rolling hashing
int hashfunction(char *word);

// Creates a new hash node for the given word
hashnode *createhashnode(char *word);

// Inserts a word into the dictionary
void insert_in_dictionary(dictionary *d, char *word);

// Calculates the Levenshtein distance between two strings
int levenshtein(const char *src, const char *dest);

// Computes the longest common prefix length between two words
int longest_common_prefix(const char *word1, const char *word2);

// Computes edit distances for all dictionary words relative to the input word
// and collects the top suggestions
void compute_editdistance(dictionary *d, char *word, hashnode *suggestions[], int *suggestion_count);

// Finds suggestions for a misspelled word based on Levenshtein distance and LCP
void find_suggestions(dictionary *d, char *word, hashnode *suggestions[], int *suggestion_count);

// Displays the top suggestions for a misspelled word
void display_suggestions(hashnode *suggestions[], int suggestion_count);

// Searches for a word in the dictionary, provides suggestions if misspelled
void search_in_dictionary(dictionary *d, char *key, hashnode *suggestions[], int *suggestion_count);







