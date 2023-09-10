# Specify the source file and output file name
SRC = Nevatia_Akhilesh-Assignment-1.01.c
OUT = PokeMap

# Default target
all: $(OUT)

# Compile the source code
$(OUT): $(SRC)
	gcc $(SRC) -o $(OUT)

# Run the program
run: $(OUT)
	./$(OUT)

# Clean up generated files
clean:
	rm -f $(OUT)