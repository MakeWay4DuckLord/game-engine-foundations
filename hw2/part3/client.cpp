#include <zmq.hpp>
#include <string.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {

    //connect to the server with a req socket
    zmq::context_t context(1);
    const char * reqProtocol = "tcp://127.0.0.1:5555";
    const char * subProtocol = "tcp://127.0.0.1:5556";
    zmq::socket_t mySocket (context, zmq::socket_type::req);
    zmq::socket_t subSocket (context, zmq::socket_type::sub);
    mySocket.connect(reqProtocol);
    subSocket.connect(subProtocol);

    subSocket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   if(!subSocket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }

    //do intitial req
    json initReq = {
        {"id", -1},
        {"iteration", 0},
    };

    std::string msgStr = initReq.dump();
    zmq::message_t msg(msgStr.c_str(), msgStr.length());
    mySocket.send(msg, zmq::send_flags::none);

    //dont need to do anything with this reply
    zmq::message_t reply;
    mySocket.recv(reply, zmq::recv_flags::none);
    mySocket.close();

    //now just print out every message published to subscriber socket
    while(true) {
        zmq::message_t data;
        subSocket.recv(data, zmq::recv_flags::none);
        json client = json::parse(data.to_string());
        std::cout << "Client " << client["id"] << ": Iteration " << client["iteration"] << std::endl;
    }

    subSocket.close();
    context.close();    

    return 0;
}