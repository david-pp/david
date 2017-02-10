#include <iostream>
#include <string>
#include "zhelper.h"

void client()
{
    zmq::context_t context(1);

    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5559");

    for( int request = 0 ; request < 10 ; request++) {

        s_send (requester, "Hello");
        std::string string = s_recv (requester);

        std::cout << "Received reply " << request
                  << " [" << string << "]" << std::endl;
    }
}

void worker()
{
    zmq::context_t context(1);

    zmq::socket_t responder(context, ZMQ_REP);
//    responder.bind("tcp://*:5559");

    responder.connect("tcp://localhost:5560");
    while(1)
    {
        s_dump(responder);
        //  Wait for next request from client
//        std::string string = s_recv (responder);

//        std::cout << "Received request: " << string << std::endl;

        // Do some 'work'
        sleep (1);

        //  Send reply back to client
        s_send (responder, "World");

    }
}

void broker()
{
    //  Prepare our context and sockets
    zmq::context_t context(1);
    zmq::socket_t frontend (context, ZMQ_ROUTER);
    zmq::socket_t backend (context, ZMQ_DEALER);

    frontend.bind("tcp://*:5559");
    backend.bind("tcp://*:5560");

    //  Initialize poll set
    zmq::pollitem_t items [] = {
            { frontend, 0, ZMQ_POLLIN, 0 },
            { backend,  0, ZMQ_POLLIN, 0 }
    };

    //  Switch messages between sockets
    while (1) {
        zmq::message_t message;
        int more;               //  Multipart detection

        zmq::poll (&items [0], 2, -1);

        if (items [0].revents & ZMQ_POLLIN) {
//            s_dump(frontend);
            while (1) {
                //  Process all parts of the message
                frontend.recv(&message);
                s_dumpmsg(message);
                size_t more_size = sizeof (more);
                frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                backend.send(message, more? ZMQ_SNDMORE: 0);

                if (!more)
                    break;      //  Last message part
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
                //  Process all parts of the message
                backend.recv(&message);
                s_dumpmsg(message);
                size_t more_size = sizeof (more);
                backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                frontend.send(message, more? ZMQ_SNDMORE: 0);
                if (!more)
                    break;      //  Last message part
            }
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 2) return 0;

    std::string op = argv[1];

    if ("client" == op)
        client();
    else if ("worker" == op)
        worker();
    else if ("broker" == op)
        broker();
    return 0;
}
