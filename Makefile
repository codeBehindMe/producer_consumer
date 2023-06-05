IDIR=./
STD :=c++14
CXX=g++
CXXFLAGS=-I$(IDIR) -Wall -Wextra -pedantic-errors -std=$(STD) -O2 -pthread

build: *.cpp
	$(CXX) -o producer_consumer.o $(CXXFLAGS) *.cpp

.PHONY: clean

clean:
	rm -f producer_consumer.o

run:
	./producer_consumer.o $(ARGS)