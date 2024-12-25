#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

void init_array_of_selected_text()
{

    arr_selected_text.A = (char **)malloc(sizeof(char *) * SIZE);
    arr_selected_text.len = 0;
    return;
}

void append_array_of_selected_text()
{
    arr_selected_text.A[arr_selected_text.len] = (char *)malloc(strlen(info.selected_text) + 1);
    strcpy(arr_selected_text.A[arr_selected_text.len], info.selected_text);
    arr_selected_text.len++;
    return;
}

void on_selection_changed(GtkTextBuffer *buffer, gpointer text_view)
{
    GtkTextIter start, end;
    char *selected_text;
    if (arr_selected_text.len == 0)
    {
        
        init_array_of_selected_text();
    }

    // Check if any text is selected
    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
    {
        selected_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        info.selected_text = malloc(strlen(selected_text) + 1);
        strcpy(info.selected_text, selected_text);
        
        append_array_of_selected_text();
      
        g_free(selected_text);
    }
}


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


