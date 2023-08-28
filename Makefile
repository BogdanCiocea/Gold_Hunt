# Compiler to use
CC = gcc

# Compilation flags
CFLAGS = -Wall -Wextra -g

# Source file
SRC = gold.c

# Output executable name
OUT = gold

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

.PHONY: all clean
