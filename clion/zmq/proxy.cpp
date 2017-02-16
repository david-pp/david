#include <iostream>
#include <string>
#include "zhelper.h"

void client_sync()
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

void client_async()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_DEALER);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5559");

    //  Initialize poll set
    zmq::pollitem_t items [] = {
            { socket, 0, ZMQ_POLLIN, 0 },
    };

    int count = 0;

    //  Switch messages between sockets
    while (1)
    {
        zmq::poll (&items [0], 1, 1000);

        if (items [0].revents & ZMQ_POLLIN) {
            s_dump(socket);
        }

        zmq::message_t empty;
        socket.send(empty, ZMQ_SNDMORE);

        std::ostringstream msg;
        msg << "Hello:" << count++;
        s_send(socket, msg.str());

        std::cout << "Sending ... " << msg.str() << std::endl;
    }
}

void worker_sync()
{
    zmq::context_t context(1);

    zmq::socket_t responder(context, ZMQ_REP);
//    responder.bind("tcp://*:5559");

    responder.connect("tcp://localhost:5560");
    while(1)
    {
        s_dump(responder);

        // Do some 'work'
//        sleep (1);
        std::cout << "work...." << std::endl;

        //  Send reply back to client
        s_send (responder, "World");

    }
}

void worker_async()
{
    zmq::context_t context(1);

    zmq::socket_t responder(context, ZMQ_ROUTER);
//    responder.bind("tcp://*:5559");

    responder.connect("tcp://localhost:5560");
    while(1)
    {
        zmq::message_t id;
        zmq::message_t empty;
        zmq::message_t request;

        responder.recv(&id);
        responder.recv(&empty);
        responder.recv(&request);
//        s_dump(responder);

        // Do some 'work'
//        sleep (1);
        std::cout << "work...." << std::endl;

        //  Send reply back to client
        responder.send(id, ZMQ_SNDMORE);
        responder.send(empty, ZMQ_SNDMORE);
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

    if ("c1" == op)
        client_sync();
    else if ("c2" == op)
        client_async();
    else if ("w1" == op)
        worker_sync();
    else if ("w2" == op)
        worker_async();
    else if ("b" == op)
        broker();
    return 0;
}
