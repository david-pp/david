CXXFLAGS=-I/usr/include -I/usr/local/include
CXXLIBS=-L/usr/local/lib -llua
CC=g++

all: mylua
	
mylua: mylua.cpp
	$(CC) -o $@ $<  $(CXXFLAGS) $(CXXLIBS)
