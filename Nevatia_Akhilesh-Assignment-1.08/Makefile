CC = gcc
CXX = g++
ECHO = echo
RM = rm -f


CFLAGS = -Wall -Werror -ggdb -funroll-loops 
CXXFLAGS = -Wall -Werror -ggdb -funroll-loops -std=c++11  # Added -std=c++11

LDFLAGS = -lncurses

BIN = poke327
OBJS = poke327.o heap.o character.o io.o db_parse.o pokemon.o

all: $(BIN) 

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out




