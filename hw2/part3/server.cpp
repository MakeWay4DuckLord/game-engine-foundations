#include <zmq.hpp>
#include <string>
#include<unistd.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {

    json serverData;

    std::vector<json> clients;
    int clientCount = 0;

    //create and connect to the socket
    zmq::context_t context(1);
    zmq::socket_t mySocket (context, zmq::socket_type::rep);
    zmq::socket_t pubSocket (context, zmq::socket_type::pub);
    // const char * protocol = "ipc:///tmp/test";
    const char * repProtocol = "tcp://127.0.0.1:5555";
    const char * pubProtocol = "tcp://127.0.0.1:5556";

    mySocket.bind(repProtocol);
    pubSocket.bind(pubProtocol);

    if(!pubSocket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }
    
    int iteration = 0;

      json j = {
        {"iteration", 0},
      };
    while(true) {
      zmq::message_t req;

      zmq::recv_result_t status = mySocket.recv(req, zmq::recv_flags::dontwait);
      if(status.has_value()) {
        clients.push_back(json::parse(req.to_string()));
        clients[clients.size() - 1]["id"] = clients.size();
        mySocket.send(zmq::message_t(clients[clients.size() -1].dump().c_str(), clients[clients.size() -1].dump().length()), zmq::send_flags::none);
      }
      for(int i = 0; i < clients.size(); i++) {
        int iteration = clients.at(i)["iteration"];
        clients.at(i)["iteration"] = iteration + 1;
      }

      for(json client : clients) {
        zmq::message_t data(client.dump().c_str(), client.dump().length());
        pubSocket.send(data, zmq::send_flags::none);
      }
      sleep(1);
      }

    //close whats gotta close and exit
    pubSocket.close();
    mySocket.close();
    context.close();
    return 0;
}