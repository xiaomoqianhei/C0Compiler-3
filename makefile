CFLAGS = -Wall -g 
SRC=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)

a: $(HEADERS) $(SRC)
	g++ -std=c++11 -o $@ $^ $(CFLAGS)
