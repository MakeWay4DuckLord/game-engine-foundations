#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include "clientData.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {

    int clients = 0;

    json serverData;

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
    

    while(true) {
    //now listen
    zmq::message_t request;
    mySocket.recv(request, zmq::recv_flags::none);

    json j = json::parse(request.to_string());

    //simulate some work
    sleep(1);

    //update data
    int id = (int) j["id"];
    if(id == 0) {
      j["id"] = ++clients;
    }

    int iterations = (int) j["iteration"];
    j["iteration"] = iterations + 1;

    //create message
    std::string rep = j.dump(); 
    zmq::message_t reply(rep.c_str(), rep.length());

    //send message 
    mySocket.send(reply, zmq::send_flags::none);

    }

    //close whats gotta close and exit
    mySocket.close();
    context.close();
    return 0;
}
