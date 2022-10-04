#include <zmq.hpp>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "clientData.h"

//helper method to print out data from clientData struct
void report(clientData* data) {
    std::cout << "Client " << data->id << ":"
        << " Iteration " << data->iteration << std::endl;
}

int main(void) {
    
    //initialize this clients data
    clientData *myData = (clientData*) malloc(sizeof(clientData));
    myData->id = 0;
    myData->iteration = 0;

    //connect to the server with a req socket
    zmq::context_t context(1);
    const char * protocol = "tcp://127.0.0.1:5555";
    zmq::socket_t mySocket (context, zmq::socket_type::req);
    mySocket.connect(protocol);

    //send request
    for(int i = 1; i <= 10; i++) {

        zmq::message_t request(DATA_SIZE);
        memcpy(request.data(), myData, DATA_SIZE);
        mySocket.send(request, zmq::send_flags::none);

        zmq::message_t reply;
        mySocket.recv(reply, zmq::recv_flags::none);
        memcpy(myData, reply.data(), DATA_SIZE);
        myData->iteration = i;
        report(myData);
        
    }

    mySocket.close();
    context.close();    

    return 0;
}