#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#define SIZE 64
#define MAX_WORD_LENGTH 50
#define MAX_SIZE 34003
#define MAX_SUGGESTIONS 10

//---------------------------------spell check struct start
// Represents a node in the dictionary
typedef struct hashnode
{
    char word[MAX_WORD_LENGTH];
    struct hashnode *next;
    int levenshtein;
    int lcp;
} hashnode;

// Represents the array of pointers pointing to the nodes in the dictionary
typedef struct dictionary
{
    hashnode *h[MAX_SIZE];
} dictionary;


// Represents the array of pointers pointing to the nodes in the Suggestions
hashnode *suggestions[MAX_SUGGESTIONS];
int suggestion_count = 0;

//Global Declaration of Dictionary
dictionary d;

//---------------------------------spell check stuct end

//---------------------------------trie node for search start
typedef struct trienode
{
    struct trienode *child[26];
    bool isendofword;
} trienode;

typedef struct trienode *root;
root r;


//----trie node for search end
//--------------------dll struct-------------------------
typedef struct word
{
    struct word *prev_word;
    char *str;
    struct word *next_word;
} word; // dll with each node being a character

typedef struct text
{
    struct text *prev_line;
    word *line;
    struct text *next_line;
} text;
typedef struct text *Text;

// Text Text_Buffer_Traverser;
word *word_list_head = NULL;

Text Text_Buffer;

typedef struct replace_info
{
    char *selected_text;
} replace_info;

replace_info info;

/*
typedef struct cursor{
    int pos;
    struct line *insertion_point;

}TextCursor;
*/

typedef struct
{
    GtkTextView *text_view;
    GtkEntry *entry;
} CallbackData;

typedef struct save_iter
{
    GtkTextIter last_saved_iter;
    int len;
} save_iterator;

save_iterator iter;

typedef struct StackNode
{
    word *node;
    int is_new_line_node;
    struct StackNode *nextStackNode;
} StackNode;

struct stack
{
    StackNode *top;
};


typedef struct stack Stack; // stack required for operations like undo & redo
Stack stack;

typedef struct array_selected_text
{
    char **A;
    int len;
} array_of_selected_text;

array_of_selected_text arr_selected_text;


//----------Search function starts------------------------------
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


// -----------------------------------------trie node function end

///-----------------------------------------spellcheck function start
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

hashnode *createhashnode(char *word)
{
    hashnode *nn = (hashnode *)malloc(sizeof(hashnode));
    nn->next = NULL;
    strcpy(nn->word, word);
    nn->levenshtein = -1; // Initialize levenshtein to an invalid number
    nn->lcp = 0;
    return nn;
}

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

int longest_common_prefix(const char *word1, const char *word2)
{
    int i = 0;
    while (word1[i] && word2[i] && word1[i] == word2[i])
    {
        i++;
    }
    return i;
}

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

void display_suggestions(hashnode *suggestions[], int suggestion_count)
{
    printf("\nDid you mean?:\n");
    for (int i = 0; i < suggestion_count; i++)
    {
        printf("'%s'\t", suggestions[i]->word);
    }
}

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

///-------------------------------------------------spellcheckfunction end


// Declare labels globally for line and word counts
GtkWidget *line_count_label;
GtkWidget *word_count_label;
int no_of_words = 0;
int no_of_lines = 0;


void init_text_editor(Text *buffer)
{
    buffer = (Text *)malloc(sizeof(Text));
    *buffer = NULL;
    printf("Text Editor : Initailized\n\n\n");
    printf("Welcome to the GTK Text Editor! Below are the instructions for using the editor’s features:\n");
	printf("1. Real-Time Updates\n");
	printf("   • The number of lines and words will be displayed in real-time.\n\n");

	printf("2. Search\n");
	printf("   • Steps:\n");
	printf("     1. Save your text by clicking the Save button.\n");
	printf("     2. Enter the word to search in the Search Area.\n");
	printf("     3. Click the Search button to find the word.\n\n");

	printf("3. Undo\n");
	printf("   • Steps:\n");
	printf("     1. Save your text.\n");
	printf("     2. Click the Undo button to reverse the last change.\n\n");

	printf("4. Redo\n");
	printf("   • Steps:\n");
	printf("     1. Save your text.\n");
	printf("     2. Click the Redo button to reapply the undone change.\n\n");

	printf("5. Replace\n");
	printf("   • Steps:\n");
	printf("     1. Select the text to be replaced using the cursor.\n");
	printf("     2. Enter the replacement word in the Search Area.\n");
	printf("     3. Click the Replace button to complete the action.\n\n");

	printf("6. Delete\n");
	printf("   • Steps:\n");
	printf("     1. Type the word you want to delete in the Text Area.\n");
	printf("     2. Click the Delete button to remove all instances of the word.\n\n");

	printf("7. Save\n");
	printf("   • Save your text at any time by clicking the Save button.\n\n");

	printf("8. Highlight\n");
	printf("   • Steps:\n");
	printf("     1. Select the text to be highlighted using the cursor.\n");
	printf("     2. Click the Highlight button to apply the highlight.\n\n");

	printf("9. Spell-Check\n");
	printf("   • After clicking Save, all the input will be spellchecked with suggestions displayed in the Terminal.\n");
	printf("\n\n\n");
	    
  
 }




void init_array_of_selected_text()
{

    arr_selected_text.A = (char **)malloc(sizeof(char *) * SIZE);
    arr_selected_text.len = 0;
    return;
}

void append_array_of_selected_text()
{
    //printf("Appending selected text: %s\n", info.selected_text);

    arr_selected_text.A[arr_selected_text.len] = (char *)malloc(strlen(info.selected_text) + 1);
    strcpy(arr_selected_text.A[arr_selected_text.len], info.selected_text);
    arr_selected_text.len++;
    //printf("printing selected text array form append \n");
    //for (int i = 0; i < arr_selected_text.len; i++)
    //{
       // printf("%s ", arr_selected_text.A[i]);
    //}

    printf("\n");
    return;
}

void on_selection_changed(GtkTextBuffer *buffer, gpointer text_view)
{
    GtkTextIter start, end;
    char *selected_text;
    if (arr_selected_text.len == 0)
    {
        //printf("initialzing selected text array\n");
        init_array_of_selected_text();
    }

    /*if(!selected_text){
        return;
    }
    if(strcmp(selected_text," ") == 0){
        return;
    }
    */

    // Check if any text is selected
    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
    {
        selected_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        info.selected_text = malloc(strlen(selected_text) + 1);
        strcpy(info.selected_text, selected_text);
        //g_print("Selected text: %s\n", selected_text);
        append_array_of_selected_text();
        // g_signal_connect(replace, "clicked", G_CALLBACK(on_replace_clicked), entry, selected_text);
        g_free(selected_text);
    }
    else
    {
        //g_print("No text selected.\n");
    }
}

int isEmpty(Text buffer_ptr)
{
    if (buffer_ptr == NULL)
    {
        return 1;
    }
    return 0;
}

//------------dll functions start-----------------

word *create_word_node(char *token)
{
    word *new_word = (word *)malloc(sizeof(word));
    new_word->str = (char *)malloc(strlen(token) + 1); // Allocate space for the word string
    strcpy(new_word->str, token);                      // Copy the token into the new word's str field
    new_word->prev_word = NULL;
    new_word->next_word = NULL;
    return new_word;
}

int word_list_head_flag = 0;
word* word_list_tail = NULL;	//maintain a tail pointer to the word list easy O(1) append

void append_word(char *token)
{
    word *new_word = create_word_node(token);

    no_of_words++;

    if (word_list_head == NULL)
    {
        // If the list is empty, the new word is the first word (head)
        word_list_head = new_word;
        word_list_tail = new_word;
        word_list_head_flag = 1; // set a flag that word list is not empty
    }
    else
    {
        word_list_tail->next_word = new_word;
        new_word->prev_word = word_list_tail;
        word_list_tail = new_word;
    }
    return;
}

text *create_line_node(word *token)
{
    text *new_line = (text *)malloc(sizeof(text));
    // Allocate space for the word string
    new_line->line = token; // Copy the token into the new word's str field
    new_line->prev_line = NULL;
    new_line->next_line = NULL;
    return new_line;
}

text* text_buffer_tail = NULL;
void append_line()
{
    no_of_lines++;
    text *new_line = create_line_node(word_list_head);
    text *temp;
    word *temp_word;
    printf("\n\n\n");
    if (isEmpty(Text_Buffer))
    {
        // If the list is empty, the new line is the first line
        Text_Buffer = new_line;
        text_buffer_tail = new_line;
    }
    else
    {
        text_buffer_tail->next_line = new_line;
        new_line->prev_line = text_buffer_tail;
        text_buffer_tail = new_line;
    }
    // word_list_head = NULL;
}

void append_to_latest_line(char *token)
{
    word *new_word = create_word_node(token);
}

void remove_prev_line()
{
    text *temp = Text_Buffer;
    word *ptr;
    if (isEmpty(Text_Buffer))
    {
        return;
    }
    while (temp->next_line != NULL)
    {
        temp = temp->next_line;
    }
    ptr = temp->line;

    if (temp->prev_line != NULL)
    {
        temp->prev_line->next_line = NULL;
    }
    else
    {
        // Handle the case where it's the only line
        Text_Buffer = NULL;
        return;
    }
    // free line;
    while (ptr != NULL)
    {
        word *to_free = ptr;
        ptr = ptr->next_word;
        free(to_free); // Free each word in the line
    }
    free(temp);
}

void tokenize_words_from_input(char *input_ptr)
{
    char *temp = input_ptr;
    if (*temp == '\n')
    {
        word_list_head = NULL;
    }
    while (*temp == ' ' || *temp == '\n')
    { // remove the leading space and \n
        temp++;
    }

    while (*temp != '\0')
    {
        char word[64];
        int i = 0;

        while (*temp != '\n' && *temp != ' ' && *temp != '\0')
        {
            word[i] = *temp;
            i++;
            temp++;
        }
        if (*temp == '\0')
        {
            temp--;
        }
        word[i] = '\0';
        append_word(word);
        // insert(&r, word);
        search_in_dictionary(&d, word, suggestions, &suggestion_count);
        //printf("token: %s\n", word);

        if (*temp == '\n')
        {

            if (word_list_head != NULL && word_list_head_flag)
            {
                append_line();
                word_list_head_flag = 0;
            }
            word_list_head = NULL;
        }
        temp++;
    }
    if (word_list_head != NULL && word_list_head_flag)
    {
        append_line();
        word_list_head_flag = 0;
    }
    return;
}

void display_word_list()
{
    text *temp = Text_Buffer;
    word *temp_word;
    int i = 0;
    if (temp == NULL)
    {
        //printf("The word list is empty.\n");
        return;
    }

    //printf("Contents of the word DLL:\n");
    while (temp != NULL)
    {
        //printf("new line[%d]::\n", i);
        temp_word = temp->line; // Start with the first word in the current line
        while (temp_word != NULL)
        {
            //printf("%s ", temp_word->str);    // Print each word
            temp_word = temp_word->next_word; // Move to the next word
        }
        //printf("\n\n"); // Newline after each line
        temp = temp->next_line;
        i++; // Move to the next line//i used for indexing only
    }
    //printf("\n");
}

StackNode *remove_newest_node()
{
    text *temp = Text_Buffer;
    word *temp_word;
    // word* ptr;
    StackNode *Node = (StackNode *)malloc(sizeof(StackNode));
    if (temp->next_line == NULL && temp->line->next_word == NULL)
    {
        Node->node = temp->line;
        no_of_words--;
        no_of_lines--;
        Text_Buffer = NULL;
        Node->node->prev_word = NULL;
        Node->node->next_word = NULL;
        return Node;
    }
    while (temp->next_line != NULL)
    {
        temp = temp->next_line;
    }
    temp_word = temp->line;

    while (temp_word->next_word != NULL)
    {
        temp_word = temp_word->next_word; // Move to the next word
    }
    Node->node = temp_word;
    if (Node->node->prev_word != NULL)
    {
        temp_word->prev_word->next_word = NULL; // Detach from previous word
        Node->is_new_line_node = 0;
    }
    else
    {
        temp->prev_line->next_line = NULL; // If it's the only word, clear the line
        no_of_lines--;
        Node->is_new_line_node = 1;
        free(temp);
    }
    no_of_words--;
    Node->node->prev_word = NULL;
    Node->node->next_word = NULL;
    Node->nextStackNode = NULL;
    return Node;
}

char *get_full_text_from_list(text *head)
{
    char *buffer = (char *)malloc(10000); // Allocate enough space
    buffer[0] = '\0';                     // Initialize buffer as an empty string

    text *temp_line = head;
    while (temp_line != NULL)
    {
        word *temp_word = temp_line->line;
        while (temp_word != NULL)
        {
            strcat(buffer, temp_word->str);
            strcat(buffer, " ");
            temp_word = temp_word->next_word;
        }
        strcat(buffer, "\n"); // Move to the next line
        temp_line = temp_line->next_line;
    }

    return buffer;
}

void update_file()
{
    text *head;
    FILE *fp = fopen("outputfinal.txt", "w"); // Append to file
    if (fp == NULL)
    {
        printf("Error: Could not open file for writing.\n");
        return;
    }
    head = Text_Buffer;
    // Get the full text from the doubly linked list
    char *fullText = get_full_text_from_list(head);

    // Write the text to the file
    fputs(fullText, fp);

    // Free allocated memory for the text
    free(fullText);

    // Close the file
    fclose(fp);
    //printf("Data updated successfully.\n");
}

//---------dll functions end-----------

//---------stack functions start-----------

void init_stack()
{
    stack.top = NULL;
}
int StackEmpty()
{
    if (stack.top == NULL)
        return 1;
    return 0;
}

void push_into_stack(StackNode *newest_node)
{

    if (StackEmpty())
    {
        stack.top = newest_node;
        //printf("newest_ node_empty: %s\n", stack.top->node->str);
        return;
    }
    newest_node->nextStackNode = stack.top;
    stack.top = newest_node;
    //printf("newest_ node: %s\n", stack.top->node->str);
}

StackNode *pop_from_stack()
{
    if (StackEmpty())
    {
        printf("\nNothing to redo.\n\n");
        return NULL;
    }

    StackNode *ptr = stack.top;
    stack.top = stack.top->nextStackNode;
    /*if(stack.top != NULL){
        stack.top->nextStackNode = NULL;
    }*/

    return ptr;
}

/*void display_stack()
{
    printf("\n\nContents of stack:\n\n");
    StackNode *ptr = stack.top;
    int i = 0;
    while (ptr != NULL)
    {
        printf("stkword[%d]:%s\n", i, ptr->node->str);
        ptr = ptr->nextStackNode;
        i++;
    }
}
*/
void update_text_editor(GtkTextView *text_view)
{
    text *head;
    head = Text_Buffer;
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, get_full_text_from_list(head), -1); // Convert list to text

    // Update line and word count
    gtk_label_set_text(GTK_LABEL(line_count_label), g_strdup_printf("Lines: %d", no_of_lines));
    gtk_label_set_text(GTK_LABEL(word_count_label), g_strdup_printf("Words: %d", no_of_words));
}

/*
 * Function: undo
 * --------------
 * This function reverts the most recent change made to the text buffer.
 * It restores the last removed or modified word to its previous state and location.
 *
 * Parameters:
 * - GtkWidget *widget: The undo button or related widget triggering this function.
 * - gpointer user_data: A pointer to the GtkTextView widget containing the text buffer.
 * - Operates on the global text buffer and stack.
 *
 * Functionality:
 * 1. Retrieves the most recent word node from the text buffer.
 * 2. Pushes the node along with its "is_on_new_line" bool value to the stack to allow for redo functionality.
 * 3. Updates the text view in the interface.
 *
 * Time Complexity:
 * Let:
 * - K be the average length of the word being restored.
 * - M be the number of nodes in the undo stack.
 *
 * The function performs operations proportional to the length of the word (O(K)) 
 * for string copying and memory management. Traversing the doubly linked list to 
 * find the correct position is O(M) in the worst case. 
 * Hence, the overall time complexity is approximately O(K + M).
 */

void on_undo_button_clicked(GtkWidget *widget, GtkTextView *text_view)
{

    //printf("Undo button was clicked\n");
    if (isEmpty(Text_Buffer))
    {
        printf("Nothing to undo.\n");

        return;
    }

    // remove a word_node from the DLL and store it in the stack
    StackNode *newest_node = remove_newest_node();

    push_into_stack(newest_node);

    update_text_editor(text_view);
    update_file();
    display_word_list();
    //display_stack();
}

/*
 * Function: redo
 * --------------
 * This function reapplies the most recently undone change to the text buffer.
 * It restores the state of the text buffer to the point before the undo operation.
 *
 * Parameters:
 * - GtkWidget *widget: The redo button or related widget triggering this function.
 * - gpointer user_data: A pointer to the GtkTextView widget containing the text buffer.
 * - Operates on the global text buffer and stack.
 *
 * Functionality:
 * 1. Retrieves the most recent nodefrom the stack.
 * 2. Updates the text buffer accordingly.
 * 3. Updates the text view in the interface.
 *
 * Time Complexity:
 * Let:
 * - K be the average length of the word being reapplied.
 * - M be the number of nodes in the redo stack.
 *
 * Similar to undo, the function operates on the word length for memory operations (O(K)) 
 * and traverses the doubly linked list to locate the position (O(M)).
 * Hence, the overall time complexity is approximately O(K + M).
 */


void on_redo_button_clicked(GtkWidget *widget, gpointer text_view)
{
    //printf("Redo button was clicked\n");

    StackNode *newest_node = pop_from_stack();

    if (newest_node == NULL)
    {
        return;
    }
    if (isEmpty(Text_Buffer))
    {
        //printf("Empty\n\n");
        text *new_line = (text *)malloc(sizeof(text));
        new_line->line = (word *)malloc(sizeof(word));
        new_line->line = newest_node->node;
        new_line->next_line = NULL;
        new_line->prev_line = NULL;
        Text_Buffer = new_line;
        no_of_lines++;
        no_of_words++;
        update_text_editor(text_view);
        update_file();
        display_word_list();
        //display_stack();
        return;
    }
    text *temp_line = Text_Buffer;
    word *temp_word;
    while (temp_line->next_line != NULL)
    {
        temp_line = temp_line->next_line;
    }

    if (newest_node->is_new_line_node == 0)
    {
        temp_word = temp_line->line;
        while (temp_word->next_word != NULL)
        {
            temp_word = temp_word->next_word;
        }
        temp_word->next_word = newest_node->node;
        newest_node->node->prev_word = temp_word;
    }
    else
    {

        text *new_line = (text *)malloc(sizeof(text));
        new_line->line = (word *)malloc(sizeof(word));
        new_line->line = newest_node->node;
        new_line->next_line = NULL;
        new_line->prev_line = NULL;
        temp_line->next_line = new_line;
        new_line->prev_line = temp_line;
        no_of_lines++;
    }
    no_of_words++;
    update_text_editor(text_view);
    update_file();
    display_word_list();
    //display_stack();
}
//---------stack functions end-----------

//---------save function starts----------


/*
 * Function: save_file
 * -------------------
 * This function saves the newly inputted text from the GTK text buffer to a doubly linked list (DLL) 
 * and writes the entire text buffer content to a file named "outputfinal.txt".
 *
 * Parameters:
 * - GtkWidget *widget: The save button or related widget triggering this function (unused in this code).
 * - gpointer user_data: A pointer to the GtkTextView widget containing the text buffer.
 *
 * Functionality:
 * 1. Retrieves the current text buffer and the iterator pointing to the end of the buffer.
 * 2. Extracts the newly inputted text (since the last save) using `iter.last_saved_iter`.
 * 3. Checks if new text exists and tokenizes it into words, adding them to the doubly linked list.
 * 4. Displays the updated word list in the doubly linked list.
 * 5. Opens the file "outputfinal.txt" for writing:
 *    - Retrieves the full text from the doubly linked list.
 *    - Writes the text to the file.
 *    - Frees allocated memory for the text and closes the file.
 * 6. Updates the line and word count and displays them.
 * 7. Updates the `iter.last_saved_iter` to point to the current end of the buffer.
 * 8. Updates `iter.len` to store the total number of characters in the buffer.
 *
 * Edge Cases Handled:
 * - Checks if new text exists before attempting to save.
 * - Handles file I/O errors gracefully with an error message.
 * - Frees dynamically allocated memory for safety.
 *
 * Time Complexity:
 * Let:
 * - T be the size of the new text since the last save.
 * - N be the total number of words in the text buffer.
 *
 * The function performs operations proportional to:
 * - Extracting text: O(T)
 * - Tokenizing text into words and adding to DLL: O(T)
 * - Writing the entire text from the doubly linked list to the file: O(N)
 * 
 * Hence, the overall time complexity is approximately O(T + N).
 */

void save_file(GtkWidget *widget, gpointer user_data)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    GtkTextIter end;
    text *head;

    // creating a space after saving
    // char* space = " ";

    // Get the current end of the buffer
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_get_start_iter(buffer, &iter.last_saved_iter);

    // Extract the text between the last saved position and the current end
    char *text = gtk_text_buffer_get_text(buffer, &iter.last_saved_iter, &end, FALSE);
    char *ptr = text;

    // Check if there's new text to save
    if (text && strlen(text) > 0)
    {
        if (iter.len > 0)
        {
            for (int i = 0; i < iter.len; i++)
            {
                ptr++;
            }
        }

        //printf("NEW INPUTED TEXT: %s\n", ptr);

        // tokenizing the string and puttings it into the DLL
        tokenize_words_from_input(ptr);
        display_word_list();

        // Save the text to a file
        FILE *fp = fopen("outputfinal.txt", "w"); // write to the file
        if (fp == NULL)
        {
            printf("Error: Could not open file for writing.\n");
            return;
        }
        head = Text_Buffer;
        // Get the full text from the doubly linked list
        char *fullText = get_full_text_from_list(head);

        // Write the text to the file
        fputs(fullText, fp);

        // Free allocated memory for the text
        free(fullText);

        // Close the file
        fclose(fp);
        //printf("Data saved successfully.\n");
        
        //update the count
        //printf("No.of words: %d\n", no_of_words);
        //printf("No.of lines: %d\n", no_of_lines);
        // Free the allocated memory for the text
        g_free(text);

        // Use gtk_text_iter_assign to properly update last_saved_iter to the current end
        gtk_text_iter_assign(&iter.last_saved_iter, &end);

        // Update iter.len to store the total number of characters
        iter.len = gtk_text_iter_get_offset(&end); // Store the offset as the new length
    }
    else
    {
        printf("No new text to save\n");
    }
}

//-------------save function ends------------------------

//---------replace funcs starts--------------------------

/*
 * Function: replace
 * -----------------
 * This function replaces all occurrences of the selected text with the input text.
 * It traverses the text buffer and updates each word that matches the selected text.
 *
 * Parameters:
 * - const char *input_text: The new text that will replace the selected text.
 * - char *selected_text: The text that will be replaced by the input text.
 *
 * Functionality:
 * 1. Traverses the entire text buffer line by line.
 * 2. For each word in the buffer, it checks if it matches the selected text.
 * 3. If a match is found, the word is replaced with the input text.
 * 4. The memory for the old word is freed and new memory is allocated for the replaced word.
 * 5. After replacements, the word list is displayed and the file is updated.
 *
 * Time Complexity:
 * Let:
 * - L be the number of lines in the text buffer.
 * - W be the number of words in each line on average.
 * - K be the average length of the selected and replacement text.
 *
 * The function traverses each word in the buffer (L * W), checks for a match (which is O(K) due to the `strcmp` operation),
 * and replaces it if necessary. The memory allocation and copying operations also take O(K) time.
 * Hence, the overall time complexity is approximately O(L * W * K).
 */

void replace(const char *input_text, char *selected_text)
{
    text *temp = Text_Buffer;
    word *temp_word = word_list_head;
    char *input = (char *)input_text;

    while (temp != NULL)
    {
        temp_word = temp->line;
        while (temp_word != NULL)
        {
            if (strcmp(temp_word->str, selected_text) == 0)
            {
                free(temp_word->str);
                temp_word->str = (char *)malloc(strlen(input) + 1); // Allocate space for the word string
                strcpy(temp_word->str, input);
            }
            temp_word = temp_word->next_word; // move to the next word
        }
        temp = temp->next_line; // move to next line
    }

    // display_word_list();
    update_file();
}

/*
 * Function: on_replace_clicked
 * ------------------------------
 * This function is a callback for the "replace" button click event.
 * It replaces the selected text in the text buffer with the text entered by the user.
 *
 * Parameters:
 * - GtkTextBuffer *widget: The text buffer associated with the text view.
 * - gpointer user_data: User-specific data passed to the callback (in this case, callback data containing the GtkEntry and GtkTextView).
 *
 * Functionality:
 * 1. Retrieves the text entered by the user in the GtkEntry widget.
 * 2. Retrieves the selected text (from a global structure, `info.selected_text`).
 * 3. Calls the `replace` function to replace the selected text with the user input.
 * 4. Updates the text editor (GtkTextView) to reflect the changes.
 *
 */

void on_replace_clicked(GtkTextBuffer *widget, gpointer user_data)
{
    CallbackData *data = (CallbackData *)user_data;

    // Access the GtkEntry pointers

    const gchar *entry_text = gtk_entry_get_text(data->entry);

    // printf("User input from replace func : %s\n", entry_text);
    // printf("Selected text from replace func : %s\n", info.selected_text);
    replace(entry_text, info.selected_text);
    update_text_editor(data->text_view);
}

//---------replace funcs ends----------------------------

//---------highlight funcs starts------------------------

/*
 * Function: on_highlight_clicked
 * ------------------------------
 * This function is a callback for the "highlight" button click event.
 * It highlights the selected words in the text buffer with a yellow background.
 *
 * Parameters:
 * - GtkTextBuffer *buffer: The text buffer associated with the text view.
 * - GtkTextView *text_view: The GtkTextView widget that displays the text buffer.
 *
 * Functionality:
 * 1. Retrieves the text buffer from the text view.
 * 2. Checks if the "highlight" tag exists in the tag table and creates it if not.
 * 3. Clears any existing highlights in the text buffer.
 * 4. Loops through each selected word and highlights all occurrences in the text buffer.
 *
 * Time Complexity:
 * Let:
 * - L be the number of lines in the text buffer.
 * - W be the number of selected words.
 * - K be the average length of the words being searched.
 *
 * The function iterates over all selected words (W) and for each word, it searches through
 * the entire text buffer, which has L lines, and for each line, it compares the word with
 * portions of text of length K.
 *
 * The time complexity is O(L * W * K), where:
 * - L: the number of lines in the buffer
 * - W: the number of selected words
 * - K: the average length of the words being highlighted
 */

void on_highlight_clicked(GtkTextBuffer *buffer, GtkTextView *text_view)
{

    // Get the buffer from the text view
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    if (!buffer)
    {
        printf("Failed to get text buffer from text view.\n");
        return;
    }

    // Print the selected text array for debugging
    /*
    printf("Selected text array is:\n");
    for (int i = 0; i < arr_selected_text.len; i++)
    {
        printf("%s\n", arr_selected_text.A[i]);
    }
    */
    // Retrieve the tag table and check if the highlight tag exists
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
    GtkTextTag *highlight_tag = gtk_text_tag_table_lookup(tag_table, "highlight");

    // Create the tag if it doesn't exist
    if (!highlight_tag)
    {
        highlight_tag = gtk_text_buffer_create_tag(
            buffer, "highlight", "background", "blue", NULL);
        if (!highlight_tag)
        {
            printf("Failed to create highlight tag.\n");
            return;
        }
    }

    // Clear existing highlights by removing the tag from the entire buffer
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_remove_tag_by_name(buffer, "highlight", &start, &end);

    // Loop over each word to highlight
    for (int i = 0; i < arr_selected_text.len; i++)
    {
        const char *word = arr_selected_text.A[i];
        gtk_text_buffer_get_start_iter(buffer, &start); // Reset start iterator for each word

        // Highlight all occurrences of the current word
        gboolean found;
        while (TRUE)
        {
            found = gtk_text_iter_forward_search(
                &start, word, GTK_TEXT_SEARCH_TEXT_ONLY,
                &start, &end, NULL);
            if (!found)
            {
                break; // No more matches for this word
            }

            // Apply the highlight tag to the match
            gtk_text_buffer_apply_tag(buffer, highlight_tag, &start, &end);

            // Move the start iterator to the end of the current match to continue searching
            start = end;
        }
        // printf("Highlighted occurrences of: %s\n", word);
    }
}

//--------highlight funcs ends----------------------------

//---------delete funcs starts----------------------------

/*
 * Function: delete
 * -----------------
 * Deletes all occurrences of a word from a text buffer.
 *
 * Parameters:
 * - const char *input_text: The word to be deleted from the text buffer.
 *
 * Functionality:
 * 1. Traverses the text buffer line by line and word by word.
 * 2. Compares each word with the input text using strcmp().
 * 3. If a match is found:
 *    - Updates the links of the previous and next words to bypass the matched word.
 *    - Frees the memory of the matched word.
 *    - Updates the line or text buffer pointers as needed when removing words or lines.
 * 4. Reduces the counts for the number of words and lines in the buffer as applicable.
 * 5. Displays the updated word list and writes the changes back to the file.
 *
 * Time Complexity:
 * Let:
 * - L be the number of lines in the text buffer.
 * - W be the average number of words per line.
 *
 * The function iterates over all lines (L) and, for each line, traverses all words (W).
 * The strcmp() function is called for each word to compare strings, which takes O(K), where K is the average length of * the words.
 *
 * Overall time complexity: O(L * W * K).
 * This assumes the text buffer is stored as a doubly linked list for lines and words.
 *
 */

void delete(const char *input_text)
{
    text *temp = Text_Buffer;
    word *temp_word = word_list_head;
    char *input = (char *)input_text;

    while (temp != NULL)
    {
        temp_word = temp->line;
        while (temp_word != NULL)
        {
            if (strcmp(temp_word->str, input) == 0)
            {
                if (temp_word->prev_word)
                {
                    temp_word->prev_word->next_word = temp_word->next_word;
                }
                else
                {
                    if (temp->next_line == NULL && temp->line->next_word == NULL)
                    {
                        free(temp->line);
                        no_of_lines--;
                        Text_Buffer = NULL;
                    }
                    else
                    {
                        temp->line = temp_word->next_word;
                        if (temp->line == NULL)
                        {
                            no_of_lines--;
                            temp->prev_line->next_line = NULL;
                            free(temp->line);
                        }
                    }
                }
                free(temp_word);
                no_of_words--;
            }
            temp_word = temp_word->next_word; // move to the next word
        }
        temp = temp->next_line; // move to next line
    }

    // display_word_list();
    update_file();
}

/*
 * This function is a callback for the "delete" button click event.
 * Parameters:
 * - GtkTextBuffer *widget: Pointer to the GtkTextBuffer associated with the text editor.
 * - gpointer user_data: Pointer to user-defined data, which is cast to a CallbackData structure.
 *
 * Functionality:
 * 1. The user_data is cast to a CallbackData structure to access the required pointers.
 * 2. Retrieves the user input from a GtkEntry widget using gtk_entry_get_text().
 * 3. Calls the delete function with the retrieved text as an argument to perform deletion.
 * 4. Updates the text editor view by calling the update_text_editor() function.
 */

void on_delete_clicked(GtkTextBuffer *widget, gpointer user_data)
{
    CallbackData *data = (CallbackData *)user_data;

    // Access the GtkEntry pointers

    const gchar *entry_text = gtk_entry_get_text(data->entry);

    // printf("User input from delete func : %s\n", entry_text);
    delete (entry_text);
    update_text_editor(data->text_view);
}

//---------delete funcs ends----------------------------

//---------count lines and words------------------

void count_lines_and_words(GtkTextBuffer *buffer, int *num_lines, int *num_words)
{
    // GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;

    // Get the start and end iterators of the buffer
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Get the text from the buffer
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Initialize counters
    *num_lines = 0;
    *num_words = 0;

    if (text)
    {
        // Count lines
        for (const gchar *p = text; *p; p++)
        {
            if (*p == '\n')
            {
                (*num_lines)++;
            }
        }
        (*num_lines)++; // Add one for the last line if not empty

        // Count words
        gboolean in_word = FALSE;
        for (const gchar *p = text; *p; p++)
        {
            if (isspace(*p))
            {
                in_word = FALSE;
            }
            else if (!in_word)
            {
                in_word = TRUE;
                (*num_words)++;
            }
        }

        g_free(text);
    }
}

// Callback to update the labels
void update_line_word_count(GtkTextBuffer *buffer, gpointer user_data)
{
    GtkWidget **labels = (GtkWidget **)user_data;
    // GtkTextView *text_view = GTK_TEXT_VIEW(gtk_text_buffer_get_text_view(buffer));

    int num_lines, num_words;
    count_lines_and_words(buffer, &num_lines, &num_words);

    gchar *line_text = g_strdup_printf("Lines: %d", num_lines);
    gchar *word_text = g_strdup_printf("Words: %d", num_words);

    gtk_label_set_text(GTK_LABEL(labels[0]), line_text);
    gtk_label_set_text(GTK_LABEL(labels[1]), word_text);

    g_free(line_text);
    g_free(word_text);
}

int main(int argc, char *argv[])
{
	// Initialising Trie for Search Functionality
    init_trie(&r);
    
	// Initialising Dictionary for Spell-Checking Functionality
    init(&d);
    char word[MAX_WORD_LENGTH];
    // Opens the dictionary file, reads the file and inserts each word into the dictionary.
    FILE *fp = fopen("dictionary.txt", "r");
    if (fp == NULL)
    {
        printf("Couldn't open dictionary\n");
        return 1;
    }
    else
    {
        while (fscanf(fp, "%s", word) != EOF)
        {
            insert_in_dictionary(&d, word);
        }
        fclose(fp);
    }

    GtkWidget *window, *text_view, *top_box, *box, *undo, *redo, *search, *entry, *replace, *save, *delete, *highlight;
    GtkTextBuffer *buffer;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box for layout
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create a horizontal box for line and word count labels
    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(box), top_box, FALSE, FALSE, 5);

    // Initialize line and word count labels
    line_count_label = gtk_label_new("Lines: 0");
    word_count_label = gtk_label_new("Words: 0");

    // Align line count to the left and word count to the right
    gtk_box_pack_start(GTK_BOX(top_box), line_count_label, FALSE, FALSE, 5);
    gtk_box_pack_end(GTK_BOX(top_box), word_count_label, FALSE, FALSE, 5);
    // Create a text view widget (the text area)
    text_view = gtk_text_view_new();

    // Get the buffer from the text view
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Initialize a DLL to store the data in its nodes
    init_text_editor(&Text_Buffer);

    // Set text wrapping for the text view
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    GtkWidget *labels[2] = {line_count_label, word_count_label};
    g_signal_connect(buffer, "changed", G_CALLBACK(update_line_word_count), labels);

    // Create a scrolled window and add the text view to it
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_widget_set_hexpand(scrolled_window, TRUE);

    // Add scrolled window to the vertical box
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    // Create Undo, Redo, Search, Replace, and Save buttons
    search = gtk_button_new_with_label("Search");
    undo = gtk_button_new_with_label("Undo");
    redo = gtk_button_new_with_label("Redo");
    replace = gtk_button_new_with_label("Replace");
    delete = gtk_button_new_with_label("Delete");
    save = gtk_button_new_with_label("Save");
    highlight = gtk_button_new_with_label("Highlight");

    // Create a horizontal box for buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 15);

    // Create an entry field for search/replace
    entry = gtk_entry_new(); // Single-line text input
    gtk_box_pack_start(GTK_BOX(button_box), entry, FALSE, FALSE, 15);

    // Add buttons to the button box
    gtk_box_pack_start(GTK_BOX(button_box), search, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), undo, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), redo, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), replace, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), delete, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), save, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(button_box), highlight, FALSE, FALSE, 15);

    // Set up the callback data
    CallbackData data = {GTK_TEXT_VIEW(text_view), GTK_ENTRY(entry)};

    // Connect the buttons to their respective callback functions
    g_signal_connect(undo, "clicked", G_CALLBACK(on_undo_button_clicked), text_view);
    g_signal_connect(redo, "clicked", G_CALLBACK(on_redo_button_clicked), text_view);
    g_signal_connect(search, "clicked", G_CALLBACK(on_search_clicked), entry);
    g_signal_connect(replace, "clicked", G_CALLBACK(on_replace_clicked), &data);
    g_signal_connect(delete, "clicked", G_CALLBACK(on_delete_clicked), &data);
    g_signal_connect(buffer, "notify::has-selection", G_CALLBACK(on_selection_changed), text_view);
    g_signal_connect(save, "clicked", G_CALLBACK(save_file), text_view);
    g_signal_connect(highlight, "clicked", G_CALLBACK(on_highlight_clicked), text_view);

    // Initialize stack for undo/redo operations
    init_stack();

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
