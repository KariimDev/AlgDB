CC     = gcc
CFLAGS = -Wall -Wextra -std=c89 -Iinclude

SRC_CORE = src/linkedlist.c src/tree.c src/stack.c src/recursion.c
SRC_CLI  = main.c
SRC_GUI  = src/gui.c

all: cli.exe gui.exe

cli.exe: $(SRC_CORE) $(SRC_CLI)
	$(CC) $(CFLAGS) -o $@ $^

gui.exe: $(SRC_CORE) $(SRC_GUI)
	set CHERE_INVOKING=1 && C:\msys64\usr\bin\bash.exe -lc "export PATH=/mingw64/bin:$$PATH; gcc -w -g src/gui.c src/linkedlist.c src/tree.c src/stack.c src/recursion.c -o gui.exe `pkg-config --cflags --libs gtk+-3.0`"

clean:
	del /Q cli.exe gui.exe 2>nul || true

.PHONY: all clean
