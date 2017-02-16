#include <iostream>
#include <string>
#include <thread>
#include <functional>
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


struct Monitor : public zmq::monitor_t
{
public:
    virtual void on_monitor_started()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual void on_event_connected(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl; }
    virtual void on_event_connect_delayed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_connect_retried(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_listening(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl; }
    virtual void on_event_bind_failed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_accepted(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_accept_failed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_closed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl; }
    virtual void on_event_close_failed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_disconnected(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_handshake_failed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_handshake_succeed(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }
    virtual void on_event_unknown(const zmq_event_t &event_, const char* addr_) { std::cout << addr_ << ": " << __PRETTY_FUNCTION__ << std::endl;  }

};


void monitor_run(zmq::socket_t& socket)
{
    Monitor m;
    m.monitor(socket, "inproc://monitor.rep");
}

void hwclient_dealer()
{
    using namespace std::placeholders;
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_DEALER);

    std::thread mthread(monitor_run, std::ref(socket));

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    socket.setsockopt<int>(ZMQ_SNDHWM, 10);

    std::cout << "SEND:" << socket.getsockopt<int>(ZMQ_SNDHWM) << std::endl;
    std::cout << "SEND:" << socket.getsockopt<int>(ZMQ_RCVHWM) << std::endl;


    //  Initialize poll set
    zmq::pollitem_t items [] = {
            { socket, 0, ZMQ_POLLIN, 0 },
    };

    int count = 0;

    //  Switch messages between sockets
    while (1)
    {
        zmq::message_t message;
        int more;               //  Multipart detection

        zmq::poll (&items [0], 1, 1);

        if (items [0].revents & ZMQ_POLLIN) {
            s_dump(socket);
        }

//        s_sendmore(socket, "AA");
//        s_sendmore(socket, "");
//        s_sendmore(socket, "BB");

        zmq::message_t empty;
        socket.send(empty, ZMQ_SNDMORE);

        std::ostringstream msg;
        msg << "Hello:" << count++;
        s_send(socket, msg.str());

        std::cout << "Sending ... " << msg.str() << std::endl;
    }

    mthread.join();
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


    socket.setsockopt<int>(ZMQ_SNDHWM, 10);
    socket.setsockopt<int>(ZMQ_RCVHWM, 10);

    std::cout << "SEND:" << socket.getsockopt<int>(ZMQ_SNDHWM) << std::endl;
    std::cout << "SEND:" << socket.getsockopt<int>(ZMQ_RCVHWM) << std::endl;

    std::thread mthread(monitor_run, std::ref(socket));

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
//        sleep(1);
//
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
