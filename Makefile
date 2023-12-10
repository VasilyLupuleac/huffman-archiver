.PHONY: all clean

CXX=g++
CXXFLAGS = -std=c++11 -O2 -pedantic -I src -Wall -Wextra -Werror

all: bin huffman

bin:
	mkdir -p bin

huffman: bin/main.o bin/huffman.o
	$(CXX) $(CXXFLAGS) $^ -o huffman

test: bin/test.o bin/autotest.o bin/huffman_test.o bin/huffman.o
	$(CXX) $(CXXFLAGS) $^ -o bin/test

bin/%t.o : test/%t.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/%n.o: src/%n.cpp bin
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -r bin huffman