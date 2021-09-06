CXX=g++
CXXFLAGS=-g -std=c++17
BIN=prog

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
    $(CXX) -o $(BIN) $^

%.o: %.c
    $(CXX) $@ -c $<

clean:
    rm -f *.o
    rm $(BIN)