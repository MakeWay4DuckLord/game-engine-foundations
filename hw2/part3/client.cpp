#include <zmq.hpp>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "clientData.h"

using json = nlohmann::json;

int main(void) {
    
    //create json object with data
    json data = {
        {"id", 0},
        {"iteration", 0},
    };
    
    // std::cout << data["id"] << data["iteration"] << std::endl;
    

    //connect to the server with a req socket
    zmq::context_t context(1);
    const char * protocol = "tcp://127.0.0.1:5555";
    zmq::socket_t mySocket (context, zmq::socket_type::req);
    mySocket.connect(protocol);

    while(true) {
        //send request
        std::string s = data.dump();
        zmq::message_t request(s.c_str(), s.length());
        mySocket.send(request, zmq::send_flags::none);

        //wait for reply
        zmq::message_t reply;
        mySocket.recv(reply, zmq::recv_flags::none);

        //get data out of json and print it
        data = json::parse(reply.to_string());
        std::cout << "Client " << data["id"] << ": Iteration " << data["iteration"] << std::endl;
    }

    mySocket.close();
    context.close();    

    return 0;
}