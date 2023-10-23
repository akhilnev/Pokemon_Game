

CC = gcc
CFLAGS = -Wall -Werror  -lncurses
TARGET = poke327
SOURCE = Nevatia_Akhilesh_Assignment1.05.c heap.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

