#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_WORD_LENGTH 50
#define MAX_SIZE 34003 // Represents the maximum size of the hash table- is a prime number and is chosen accordingly to have the ideal load factor 0.75
#define MAX_SUGGESTIONS 10 // Represents the maximum number of suggestions for misspelled words


// Initializes the dictionary by setting all hash table buckets to NULL
void init(dictionary *d)
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        d->h[i] = NULL;
    }
}


/*
 * Function: hashfunction
 * ----------------------
 * Computes a hash value for a given word using polynomial rolling hashing.
 *
  Parameters:
 * - char *word: A null-terminated string representing the word to hash.
 * 
 * Functionality:
 * 1. Uses a base `p` (31) and modulus `m` (MAX_SIZE) to compute the hash.
 * 2. Iterates over each character in the word:
 *    a. Converts the character to an integer relative to 'a'.
 *    b. Multiplies by powers of `p` and accumulates modulo `m`.
 *
 * Edge Cases:
 * - Handles words with only lowercase letters. May need adjustment for uppercase or special characters.
 *
 * Time Complexity:
 * O(L): Where L is the length of the word.
 */
 
 // Computes a hash value for a given word using polynomial rolling hashing
int hashfunction(char *word)
{
    const int p = 31;
    const int m = MAX_SIZE;
    long long hashval = 0;
    long long p_power = 1;

    while (*word != '\0')
    {
        hashval = (hashval + (*word - 'a' + 1) * p_power) % m;
        p_power = (p_power * p) % m;
        word++;
    }

    return (int)(hashval % m);
}


// Creates a new hash node for the given word
hashnode *createhashnode(char *word)
{
    hashnode *nn = (hashnode *)malloc(sizeof(hashnode));
    nn->next = NULL;
    strcpy(nn->word, word);
    nn->levenshtein = -1; // Initialize levenshtein to an invalid number
    nn->lcp = 0;
    return nn;
}

// Inserts a word into the dictionary
void insert_in_dictionary(dictionary *d, char *word)
{
    int hashvalue = hashfunction(word);
    hashnode *nn = createhashnode(word);
    if (d->h[hashvalue] == NULL)
    {
        d->h[hashvalue] = nn;
    }
    else
    {
        nn->next = d->h[hashvalue];
        d->h[hashvalue] = nn;
    }
}
/*
 * Function: levenshtein
 * ----------------------
 * This function calculates the Levenshtein distance between two strings.
 * The Levenshtein distance is a metric for measuring the difference between two strings,
 * defined as the minimum number of single-character edits (insertions, deletions, or substitutions)
 * required to transform one string into the other.
 *
 * Parameters:
 * - const char *src: The source string.
 * - const char *dest: The destination string.
 *
 * Returns:
 * - int: The Levenshtein distance between the two strings.
 *
 * Functionality:
 * 1. Initializes a matrix to store intermediate results of edit distances.
 * 2. Fills the first row and column to represent transforming an empty string into the other string.
 * 3. Iteratively calculates the minimum cost of edit operations for each substring combination.
 * 4. Returns the edit distance stored in the bottom-right cell of the matrix.
 *
 * Time Complexity:
 * - O(len1 * len2), where len1 and len2 are the lengths of the source and destination strings respectively.
 *
 * Space Complexity:
 * - O(len1 * len2), as the function uses a 2D matrix to store intermediate results.
 */
 
// Calculates the Levenshtein distance between two strings
int levenshtein(const char *src, const char *dest)
{
    int len1 = strlen(src) + 1;
    int len2 = strlen(dest) + 1;
    int matrix[len2][len1];

    for (int i = 0; i < len2; i++)
    {
        matrix[i][0] = i;
    }
    for (int j = 0; j < len1; j++)
    {
        matrix[0][j] = j;
    }

    for (int i = 1; i < len2; i++)
    {
        for (int j = 1; j < len1; j++)
        {
            int cost;
            if (src[j - 1] == dest[i - 1])
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }
            matrix[i][j] = fmin(matrix[i - 1][j] + 1, fmin(matrix[i][j - 1] + 1, matrix[i - 1][j - 1] + cost));
        }
    }

    return matrix[len2 - 1][len1 - 1];
}


// Computes the longest common prefix length between two words
int longest_common_prefix(const char *word1, const char *word2)
{
    int i = 0;
    while (word1[i] && word2[i] && word1[i] == word2[i])
    {
        i++;
    }
    return i;
}


// Computes edit distances for all dictionary words relative to the input word
// and collects the top suggestions
void compute_editdistance(dictionary *d, char *word, hashnode *suggestions[], int *suggestion_count)
{
    *suggestion_count = 0;

    // Iterate over the dictionary's hash table
    for (int i = 0; i < MAX_SIZE; i++)
    {
        if (d->h[i] != NULL)
        {
            hashnode *current = d->h[i];
            while (current != NULL)
            {
                // Calculate Levenshtein distance
                current->levenshtein = levenshtein(current->word, word);

                // Collect top 10 suggestions with the smallest Levenshtein distance
                if (*suggestion_count < MAX_SUGGESTIONS)
                {
                    suggestions[*suggestion_count] = current;
                    (*suggestion_count)++;
                }
                else
                {
                    // Find the suggestion with the largest Levenshtein distance
                    int max_levenshtein = suggestions[0]->levenshtein;
                    int max_index = 0;

                    for (int j = 1; j < *suggestion_count; j++)
                    {
                        if (suggestions[j]->levenshtein > max_levenshtein)
                        {
                            max_levenshtein = suggestions[j]->levenshtein;
                            max_index = j;
                        }
                    }

                    // Replace if the current node has a smaller Levenshtein distance
                    if (current->levenshtein < max_levenshtein)
                    {
                        suggestions[max_index] = current;
                    }
                }

                current = current->next;
            }
        }
    }
}


// Finds suggestions for a misspelled word based on Levenshtein distance and LCP
void find_suggestions(dictionary *d, char *word, hashnode *suggestions[], int *suggestion_count)
{
    // Compute Levenshtein distances and collect initial suggestions
    compute_editdistance(d, word, suggestions, suggestion_count);

    // Compute LCP for all suggestions
    for (int i = 0; i < *suggestion_count; i++)
    {
        suggestions[i]->lcp = longest_common_prefix(suggestions[i]->word, word);
    }

    // Sort suggestions based on Levenshtein distance first, then LCP
    for (int i = 0; i < *suggestion_count - 1; i++)
    {
        for (int j = 0; j < *suggestion_count - 1 - i; j++)
        {
            // Sort by Levenshtein distance first, then by LCP
            if (suggestions[j]->levenshtein > suggestions[j + 1]->levenshtein ||
                (suggestions[j]->levenshtein == suggestions[j + 1]->levenshtein && suggestions[j]->lcp < suggestions[j + 1]->lcp))
            {

                // Swap the suggestions
                hashnode *temp = suggestions[j];
                suggestions[j] = suggestions[j + 1];
                suggestions[j + 1] = temp;
            }
        }
    }
}

// Displays the top suggestions for a misspelled word
void display_suggestions(hashnode *suggestions[], int suggestion_count)
{
    printf("\nDid you mean?:\n");
    for (int i = 0; i < suggestion_count; i++)
    {
        printf("'%s'\t", suggestions[i]->word);
    }
}



/*
 * Function: search_in_dictionary
 * ------------------------------
 * This function checks if a given word is present in the dictionary. If the word 
 * is not found, it identifies and displays suggestions for possible corrections.
 *
 * Parameters:
 * - dictionary *d: A pointer to the dictionary, implemented as a hash table.
 * - char *key: The word to search for, provided as a null-terminated string.
 * - hashnode *suggestions[]: An array of pointers to store the top suggestions 
 *   for the misspelled word.
 * - int *suggestion_count: A pointer to an integer that holds the number of suggestions found.
 *
 * Functionality:
 * 1. Converts the input word to lowercase to ensure case-insensitive comparison.
 * 2. Computes the hash value of the word to determine its position in the hash table.
 * 3. Searches for the word in the corresponding linked list of the hash table bucket.
 * 4. If the word is found, the function exits without any further action, as the 
 *    word is spelled correctly.
 * 5. If the word is not found:
 *    a. Prints a message indicating the word is misspelled.
 *    b. Invokes the `find_suggestions` function to compute suggestions based 
 *       on Levenshtein distance and longest common prefix (LCP).
 *    c. Displays the top suggestions using the `display_suggestions` function.
 *
 * Edge Cases:
 * - Handles case-insensitivity by converting the word to lowercase.
 * - Handles words not found in the dictionary by providing relevant suggestions.
 *
 * Time Complexity:
 * - Average: O(L + K), where L is the length of the word, and K is the average size 
 *   of a hash table bucket.
 * - Worst case: O(L + N), where N is the total number of words in the dictionary 
 *   (occurs when all words hash to the same bucket).
 *
 * Space Complexity:
 * - O(1): Apart from the input parameters and local variables, no additional memory is allocated.
 */

// Searches for a word in the dictionary, provides suggestions if misspelled
void search_in_dictionary(dictionary *d, char *key, hashnode *suggestions[], int *suggestion_count)
{
    for (int i = 0; key[i]; i++)
    {
        key[i] = tolower(key[i]);
    }

    int hashvalue = hashfunction(key);
    hashnode *p = d->h[hashvalue];

    while (p != NULL)
    {
        if (strcmp(p->word, key) == 0)
        {
            // printf("Word is spelled correctly\n");
            return;
        }
        p = p->next;
    }

    // If word is not found in the linked list
    printf("\n\nThe word '%s' is misspelled\n", key);
    // Find suggestions based on Levenshtein distance and LCP
    find_suggestions(d, key, suggestions, suggestion_count);

    // Display top suggestions
    display_suggestions(suggestions, *suggestion_count);
}


