#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "header.h"

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
