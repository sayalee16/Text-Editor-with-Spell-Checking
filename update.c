#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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
