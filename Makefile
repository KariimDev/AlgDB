CC     = gcc
CFLAGS = -Wall -Wextra -std=c89 -Iinclude

SRC_CORE = src/linkedlist.c src/tree.c src/stack.c src/recursion.c
SRC_CLI  = main.c
SRC_GUI  = src/gui.c

all: cli.exe gui.exe algdb.dll web_bridge.exe

cli.exe: $(SRC_CORE) $(SRC_CLI)
	$(CC) $(CFLAGS) -o $@ $^

gui.exe: $(SRC_CORE) $(SRC_GUI)
	set CHERE_INVOKING=1 && C:\msys64\usr\bin\bash.exe -lc "export PATH=/mingw64/bin:$$PATH; gcc -w -g src/gui.c src/linkedlist.c src/tree.c src/stack.c src/recursion.c -o gui.exe `pkg-config --cflags --libs gtk+-3.0`"

algdb.dll: $(SRC_CORE)
	set CHERE_INVOKING=1 && C:\msys64\usr\bin\bash.exe -lc "export PATH=/mingw64/bin:$$PATH; gcc -shared -o algdb.dll src/linkedlist.c src/tree.c src/stack.c src/recursion.c"

web_bridge.exe: $(SRC_CORE) src/web_bridge.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	del /Q cli.exe gui.exe algdb.dll web_bridge.exe 2>nul || true

.PHONY: all clean
