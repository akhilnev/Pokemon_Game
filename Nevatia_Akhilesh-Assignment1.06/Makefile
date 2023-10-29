



CC = g++
CFLAGS = -Wall -Werror -ggdb -funroll-loops
LDFLAGS = -lncurses
TARGET = poke327
CXX_SRCS = poke327.cpp character.cpp io.cpp
C_SRCS = heap.c
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
C_OBJS = $(C_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(CXX_OBJS) $(C_OBJS)
	@echo Linking $@
	@$(CC) $^ -o $@ $(LDFLAGS)

-include $(CXX_OBJS:.o=.d) $(C_OBJS:.o=.d)

%.o: %.cpp
	@echo Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.c
	@echo Compiling $<
	@gcc $(CFLAGS) -MMD -MF $*.d -c $< -x c

clean:
	@echo Removing all generated files
	@rm -f *.o $(TARGET) *.d


# all:
# 	g++ -Wall -Werror *.c *.cpp -o a.out -lncurses

# clean:
# 	rm -f a.out


