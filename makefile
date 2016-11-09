CFLAGS = -Wall -g 
SRC=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)

a: $(HEADERS) $(SRC)
	g++ -o $@ $^ $(CFLAGS)
