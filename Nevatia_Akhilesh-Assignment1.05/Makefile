# Makefile for poke327

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -ggdb 
LDFLAGS = -lncurses

# Source files
# SRCS = Nevatia_Akhilesh_Assignment-1.05.c character.c io.c heap.c
 SRCS = testme.c heap.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXECUTABLE = poke327

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE) $(OBJS) $(SRCS:.c=.d)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean run
