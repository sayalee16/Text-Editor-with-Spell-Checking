#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

void init_trie(root *r) {
    *r = createnode();
}

void init(dictionary *d)
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        d->h[i] = NULL;
    }
}
void init_text_editor(Text *buffer)
{
    buffer = (Text *)malloc(sizeof(Text));
    *buffer = NULL;
    printf("Text Editor : Initailized\n\n\n");
    printf("Welcome to the GTK Text Editor! Below are the instructions for using the editor’s features:\n");
	printf("1. Real-Time Updates\n");
	printf("   • The number of lines and words will be displayed in real-time.\n\n");

	printf("2. Search\n");
	printf("   • Steps:\n");
	printf("     1. Save your text by clicking the Save button.\n");
	printf("     2. Enter the word to search in the Search Area.\n");
	printf("     3. Click the Search button to find the word.\n\n");

	printf("3. Undo\n");
	printf("   • Steps:\n");
	printf("     1. Save your text.\n");
	printf("     2. Click the Undo button to reverse the last change.\n\n");

	printf("4. Redo\n");
	printf("   • Steps:\n");
	printf("     1. Save your text.\n");
	printf("     2. Click the Redo button to reapply the undone change.\n\n");

	printf("5. Replace\n");
	printf("   • Steps:\n");
	printf("     1. Select the text to be replaced using the cursor.\n");
	printf("     2. Enter the replacement word in the Search Area.\n");
	printf("     3. Click the Replace button to complete the action.\n\n");

	printf("6. Delete\n");
	printf("   • Steps:\n");
	printf("     1. Type the word you want to delete in the Text Area.\n");
	printf("     2. Click the Delete button to remove all instances of the word.\n\n");

	printf("7. Save\n");
	printf("   • Save your text at any time by clicking the Save button.\n\n");

	printf("8. Highlight\n");
	printf("   • Steps:\n");
	printf("     1. Select the text to be highlighted using the cursor.\n");
	printf("     2. Click the Highlight button to apply the highlight.\n\n");

	printf("9. Spell-Check\n");
	printf("   • After clicking Save, all the input will be spellchecked with suggestions displayed in the Terminal.\n");
	printf("\n\n\n");
	    
  
 }

void init_array_of_selected_text()
{

    arr_selected_text.A = (char **)malloc(sizeof(char *) * SIZE);
    arr_selected_text.len = 0;
    return;
}

void init_stack()
{
    stack.top = NULL;
}
