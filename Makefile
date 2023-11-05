CC=g++
CFLAGS=-Wall -Werror
all: StreamShell
myshell: StreamShell.cpp command.cpp helper.cpp
	$(CC) $(CFLAGS) StreamShell.cpp Execute.cpp Parser.cpp -o StreamShell
rebuild: clean all
clean:
	rm -f StreamShell
