# CC = gcc
# CXX = g++
# ECHO = echo
# RM = rm -f

# TERM = "S2023"

# CFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM)
# CXXFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM)

# LDFLAGS = -lncurses

# BIN = poke327
# OBJS = poke327.o heap.o character.o io.o

# all: $(BIN) 

# $(BIN): $(OBJS)
# 	@$(ECHO) Linking $@
# 	@$(CXX) $^ -o $@ $(LDFLAGS)

# -include $(OBJS:.o=.d)

# %.o: %.c
# 	@$(ECHO) Compiling $<
# 	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

# %.o: %.cpp
# 	@$(ECHO) Compiling $<
# 	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

# .PHONY: all clean clobber etags

# clean:
# 	@$(ECHO) Removing all generated files
# 	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

# clobber: clean
# 	@$(ECHO) Removing backup files
# 	@$(RM) *~ \#* *pgm



CC = g++
CFLAGS = -Wall  -ggdb -funroll-loops
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


