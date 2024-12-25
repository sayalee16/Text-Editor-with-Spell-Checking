#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

// Declare labels globally for line and word counts
GtkWidget *line_count_label;
GtkWidget *word_count_label;
int no_of_words = 0;
int no_of_lines = 0;

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
