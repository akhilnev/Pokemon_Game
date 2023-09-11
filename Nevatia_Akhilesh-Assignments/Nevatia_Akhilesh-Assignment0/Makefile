CC = gcc
CFLAGS = -Wall -Werror -lm

TARGET = Assignment0
SOURCE = Assignment0.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f bigger_digger.edge.pgm
	rm -f motorcycle.edge.pgm