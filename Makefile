CC=g++
CFLAGS=-Wall -Werror

all: StreamShell

StreamShell: StreamShell.cpp Execute.cpp Parser.cpp EnvironmentVariablesManager.cpp
	$(CC) $(CFLAGS) StreamShell.cpp Execute.cpp Parser.cpp EnvironmentVariablesManager.cpp -o StreamShell

rebuild: clean all

clean:
	rm -f StreamShell
