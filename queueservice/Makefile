
CXXFLAGS=-I/usr/local/include -D_DEBUG_LOGINQUEUE
LDFLAGS=-L/usr/local/lib

DEPENDS=LoginQueue.h LoginQueue.cpp Redis.cpp Redis.h test.h

all : test b4redis httpclient

.PHONY : clean

test : test.cpp ${DEPENDS}
	g++ ${CXXFLAGS} ${LDFLAGS} -g -o $@ test.cpp LoginQueue.cpp Redis.cpp anet.c -lboost_thread -lboost_system -ljsoncpp

b4redis : b4redis.cpp ${DEPENDS}
	g++ ${CXXFLAGS} ${LDFLAGS} -g -o $@ test.cpp LoginQueue.cpp Redis.cpp anet.c -lboost_thread -lboost_system -ljsoncpp

httpclient : async_client.cpp 
	g++ ${CXXFLAGS} ${LDFLAGS} -g -o $@ $< -lboost_thread -lboost_system -ljsoncpp

clean :
	@rm -f test b4redis
