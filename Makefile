CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
SRCS = delete.c init_functions.c lines_words_count.c main.c redo.c replace.c save_in_file.c search.c spellcheck.c syntax-highlighter.c undo.c update.c
OBJS = $(SRCS:.c=.o)
HEADERS = header.h spellcheck.h

# Default target, compiles everything
all: my_program

# Rule to compile .c files into .o files
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $<

# Link object files into the final executable
my_program: ${OBJS}
	${CC} ${CFLAGS} -o my_program ${OBJS} ${LIBS} -lm

# Clean rule to remove compiled files
clean: 
	rm -f ${OBJS} my_program
