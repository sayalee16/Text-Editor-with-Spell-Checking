#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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
