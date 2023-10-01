CC = gcc
CFLAGS = -Wall -Werror -lm
TARGET = PathMaps
SOURCE = Nevatia_Akhilesh-Assignment1.03.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)