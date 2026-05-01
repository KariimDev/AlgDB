CC     = gcc
CFLAGS = -Wall -Wextra -std=c89 -Iinclude

SRC    = src/linkedlist.c src/tree.c main.c
OBJ    = $(SRC:.c=.o)
TARGET = algdb.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	del /Q $(subst /,\,$(OBJ)) $(TARGET) 2>nul || true

.PHONY: all clean
