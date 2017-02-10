#include <iostream>
#include <string>
#include "zhelper.h"

void hwclient_req()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq::message_t request (5);
        memcpy (request.data (), "Hello", 5);
        std::cout << "Sending Hello " << request_nbr << "..." << std::endl;
        socket.send (request);

        //  Get the reply.
//        zmq::message_t reply;
//        socket.recv (&reply);
//        std::cout << "Received World " << request_nbr << std::endl;

        s_dump(socket);
    }
}

void hwclient_dealer()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_DEALER);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    //  Initialize poll set
    zmq::pollitem_t items [] = {
            { socket, 0, ZMQ_POLLIN, 0 },
    };

    //  Switch messages between sockets
    while (1)
    {
        zmq::message_t message;
        int more;               //  Multipart detection

        zmq::poll (&items [0], 1, 1000);

        if (items [0].revents & ZMQ_POLLIN) {
            s_dump(socket);
        }

        std::cout << "Sending Hello ..." << std::endl;

        zmq::message_t empty;
        s_sendmore(socket, "AA");
        s_sendmore(socket, "");
        s_sendmore(socket, "BB");
        socket.send(empty, ZMQ_SNDMORE);
        s_send(socket, "Hello");
    }
}

void hwserver_rep()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
//        socket.recv (&request);
        std::cout << "Received Hello" << std::endl;
        s_dump(socket);

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        zmq::message_t reply (5);
        memcpy (reply.data (), "World", 5);
        socket.send (reply);

    }
}

void hwserver_router()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_ROUTER);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t id;
        zmq::message_t empty;
        zmq::message_t request;

        socket.recv(&id);
        socket.recv(&empty);
        socket.recv(&request);

        std::cout << "Received: " << std::string((char*)request.data(), request.size()) << std::endl;

        //  Wait for next request from client
//        socket.recv (&request);
//        std::cout << "Received Hello" << std::endl;
//        s_dump(socket);

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        socket.send(id, ZMQ_SNDMORE);
        socket.send(empty, ZMQ_SNDMORE);
        s_send(socket, "World");
    }
}

void hwserver_dealer()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_DEALER);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
//        socket.recv (&request);
//        std::cout << "Received Hello" << std::endl;
        s_dump(socket);

        //  Do some 'work'
        sleep(1);

//        //  Send reply back to client
//        zmq::message_t reply (5);
//        memcpy (reply.data (), "World", 5);
//        socket.send (reply);
    }
}


int main(int argc, const char* argv[])
{
    if (argc < 2) return 0;

    std::string ap = argv[1];

    if ("c1" == ap)
        hwclient_req();
    else if ("c2" == ap)
        hwclient_dealer();
    else if ("s1" == ap)
        hwserver_rep();
    else if ("s2" == ap)
        hwserver_router();
    else if ("s3" == ap)
        hwserver_dealer();
    return 0;
}
