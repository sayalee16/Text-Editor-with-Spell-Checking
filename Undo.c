#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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

void display_stack()
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
    display_stack();
}
