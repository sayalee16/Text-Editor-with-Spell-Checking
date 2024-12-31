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

// Declare labels globally for line and word counts
GtkWidget *line_count_label;
GtkWidget *word_count_label;
int no_of_words = 0;
int no_of_lines = 0;

// trie for search functionality
typedef struct trienode
{
    struct trienode *child[26];
    bool isendofword;
} trienode;

typedef struct trienode *root;
root r;

void delete(const char *input_text);
void on_delete_clicked(GtkTextBuffer *widget, gpointer user_data);
void init_trie(root *r);
void init(dictionary *d);
void init_text_editor(Text *buffer);
void init_array_of_selected_text();
void init_stack();
void count_lines_and_words(GtkTextBuffer *buffer, int *num_lines, int *num_words);
void update_line_word_count(GtkTextBuffer *buffer, gpointer user_data);
void on_redo_button_clicked(GtkWidget *widget, gpointer text_view);
void replace(const char *input_text, char *selected_text);
void on_replace_clicked(GtkTextBuffer *widget, gpointer user_data);
word *create_word_node(char *token);
void append_word(char *token);
text *create_line_node(word *token);
void append_line();
void tokenize_words_from_input(char *input_ptr);
void display_word_list();
void save_file(GtkWidget *widget, gpointer user_data);
void append_array_of_selected_text();
void on_selection_changed(GtkTextBuffer *buffer, gpointer text_view);
void on_highlight_clicked(GtkTextBuffer *buffer, GtkTextView *text_view);
int StackEmpty();
StackNode *pop_from_stack();
StackNode *remove_newest_node();
void push_into_stack(StackNode *newest_node);
void on_undo_button_clicked(GtkWidget *widget, GtkTextView *text_view);
char *get_full_text_from_list(text *head);
void update_text_editor(GtkTextView *text_view);
void update_file();
