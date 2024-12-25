#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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
