# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -g

# Source files
SRCS = main.c delete.c init_functions.c lines_words_count.c redo.c undo.c replace.c save_in_file.c search.c spellcheck.c syntax_highlight.c update.c

# Object files (replace .c with .o in SRCS)
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = header.h spellcheck.h

# Output executable
TARGET = my_text_editor

# Default rule to build the program
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c files to .o files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)
