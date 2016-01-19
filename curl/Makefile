
#CXXFLAGS=-I/usr/local/include 
#CXXLIBS=-L/usr/local/lib -lboost_thread -lboost_system

CXXFLAGS=-I/usr/include 
CXXLIBS=-L/usr/lib -lboost_thread-mt -lboost_system-mt

all : client simple multi debug asio

client : client.cpp HttpClient.h HttpClient.cpp
	g++ -o $@ $< HttpClient.cpp `curl-config --cflags` `curl-config --libs` $(CXXFLAGS) $(CXXLIBS)

simple : simple.c
	gcc -o $@ $< `curl-config --cflags` `curl-config --libs`	

multi : multi.cpp
	g++ -g -o $@ $< `curl-config --cflags` `curl-config --libs`	

debug : debug.c
	gcc -o $@ $< `curl-config --cflags` `curl-config --libs`	


asio : asio.cpp
	g++ -o $@ $< `curl-config --cflags` `curl-config --libs` $(CXXFLAGS) $(CXXLIBS)
