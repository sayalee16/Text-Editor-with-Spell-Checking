#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

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
