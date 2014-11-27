
all : simple multi debug asio

simple : simple.c
	gcc -o $@ $< `curl-config --cflags` `curl-config --libs`	


multi : multi.cpp
	g++ -g -o $@ $< `curl-config --cflags` `curl-config --libs`	

debug : debug.c
	gcc -o $@ $< `curl-config --cflags` `curl-config --libs`	


asio : asio.cpp
	g++  `curl-config --cflags` `curl-config --libs` -I/usr/local/include -L/usr/local/lib -lboost_thread -lboost_system -o $@ $<
