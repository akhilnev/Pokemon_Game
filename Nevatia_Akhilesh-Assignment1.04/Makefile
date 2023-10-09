CC = gcc
CFLAGS = -Wall -Werror -lm
TARGET = trainergame
SOURCE = Nevatia_Akhilesh-Assignment1.04.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS) 

run: $(TARGET)
	./$(TARGET) --numtrainers 10

clean:
	rm -f $(TARGET)