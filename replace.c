#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

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

