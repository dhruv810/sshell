# declaring variables
CC=gcc
CFLAGS=-Wall -Wextra -Werror

all: main

main: sshell.c
	$(CC) $(CFLAGS) sshell.c -o sshell


clean:
	rm -rf *o sshell
