#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include "clientData.h"

//will delete, helper method for debugging
void report(clientData* data) {
    std::cout << "Client " << data->id << ":"
        << " iteration " << data->iteration << std::endl;
}

int main(void) {

    bool running = true;

    int clients = 0;
    
    //reserve memory for data of current client
    clientData *currentData = (clientData*) malloc(sizeof(clientData));

    //create and connect to the socket
    zmq::context_t context(1);
    zmq::socket_t mySocket (context, zmq::socket_type::rep);
    // const char * protocol = "ipc:///tmp/test";
    const char * protocol = "tcp://127.0.0.1:5555";

    mySocket.bind(protocol);

    if(!mySocket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }
    
    int iterations = 5;

    while(running) {
    //now listen
    zmq::message_t request;
    mySocket.recv(request, zmq::recv_flags::none);

    //move data from message to our struct
    memcpy(currentData, request.data(), DATA_SIZE);
    report(currentData);

    //simulate some work
    sleep(1);

    //update data
    if(currentData->id == 0) {
      currentData->id = ++clients;
    }

    currentData->iteration += 1;


    //exit out if any client gets to 10 iterations
    if(currentData->iteration >= 10) {
      running = false;
    }


    //create message
    zmq::message_t reply (DATA_SIZE);    
    memcpy(reply.data(), currentData, DATA_SIZE);
    // zmq_msg_init_data(reply, currentData, DATA_SIZE)

    //send message and do a nice print for debugging
    std::cout << "sending message..." << std::endl;
    mySocket.send(reply, zmq::send_flags::none);

    }

    //close whats gotta close and exit
    mySocket.close();
    context.close();
    return 0;
}
