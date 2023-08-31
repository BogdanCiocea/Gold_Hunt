# Compiler to use
CC = gcc

# Compilation flags
CFLAGS = -Wall -Wextra -g

# Additional flags for linking with ncurses
NCURSES_LIBS = -lncurses

# Source file
SRC = gold.c

# Output executable name
OUT = gold

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(NCURSES_LIBS)

clean:
	rm -f $(OUT)

.PHONY: all clean
