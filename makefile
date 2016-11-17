CFLAGS = -Wall -g 
SRC=$(wildcard *.c)

a: $(SRC)
	gcc -o $@ $^  $(CFLAGS) 
