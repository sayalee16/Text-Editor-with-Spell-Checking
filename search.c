#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

trienode *createnode() {
    trienode *node = (trienode *)malloc(sizeof(trienode));
    node->isendofword = false;
    for (int i = 0; i < 26; i++) {
        node->child[i] = NULL;
    }
    return node;
}

void init_trie(root *r) {
    *r = createnode();
}

void insert(root *r, const char *word) {
    trienode *p = *r;
    if (*r == NULL) { // If trie is empty
        *r = createnode();
    }
    while (*word) {
        char lower_char = tolower(*word); // Handle uppercase by converting to lowercase
        int index = lower_char - 'a';
        if (p->child[index] == NULL) {
            p->child[index] = createnode();
        }
        p = p->child[index];
        word++;
    }
    p->isendofword = true;
}



void insert_in_trie()
{
    text *temp = Text_Buffer;
    word *temp_word = word_list_head;
    while (temp != NULL)
    {
        temp_word = temp->line;
        while (temp_word != NULL)
        {
            insert(&r, temp_word->str);
            temp_word = temp_word->next_word; // move to the next word
        }
        temp = temp->next_line; // move to next line
    }
}

/**
 * Function: search
 * -----------------
 * This function checks whether a given key (string) exists as a prefix or a complete word in a trie.
 * Handles both uppercase and lowercase characters by converting all input to lowercase.
 *
 * Parameters:
 * - root *r: A pointer to the root of the trie.
 * - const char *key: A null-terminated string representing the key to search for.
 *
 * Functionality:
 * 1. Starts at the root of the trie.
 * 2. Iterates over each character of the key:
 *    a. Converts the character to lowercase.
 *    b. Calculates the index of the character relative to 'a'.
 *    c. Checks if the corresponding child node exists.
 *       - If it does not, the function returns 0 (key not found).
 *    d. Moves the pointer to the child node.
 * 3. Returns 1 if the key is a prefix or a complete word in the trie.
 *
 * Edge Cases:
 * - If the root is NULL, the function returns 0.
 * - If the key is not in the trie, it returns 0.
 *
 * Time Complexity:
 * Let:
 * - K be the length of the key.
 *
 * The function performs a single pass over the key, with constant-time operations
 * for each character, such as indexing and pointer traversal.
 * Hence, the overall time complexity is O(K).
 */


int search(root *r, const char *key) {
    trienode *p = *r;
    if (*r == NULL) { // If trie is empty, return 0
        return 0;
    }
    while (*key) {
        char lower_char = tolower(*key); // Handle uppercase by converting to lowercase
        int index = lower_char - 'a';
        if (p->child[index] == NULL) {
            return 0; // Key is not a prefix of any word
        }
        p = p->child[index];
        key++;
    }
    // Return 1 if the key is a prefix of any word (even if not a complete word)
    return 1;
}


void on_search_clicked(GtkWidget *widget, gpointer entry)
{
    const char *input_text = gtk_entry_get_text(GTK_ENTRY(entry));
    insert_in_trie();
    int truthval = search(&r, input_text);
    if (truthval == 1)
    {
        printf("The key '%s' is present.\n", input_text);
    }
    else
    {
        printf("The key '%s' is not present.\n", input_text);
    }
    //printf("User input: %s\n", input_text);
}

