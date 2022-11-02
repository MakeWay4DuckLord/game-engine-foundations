#include <zmq.hpp>
#include <string>
#include <unistd.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {

    // std::vector<json> clients;
    int clientCount = 0;
    int nextId = 1;

    //create and connect to the socket
    zmq::context_t context(1);
    zmq::socket_t mySocket (context, zmq::socket_type::req);
    zmq::socket_t pubSocket (context, zmq::socket_type::pub);
    // const char * protocol = "ipc:///tmp/test";
    const char * reqProtocol = "tcp://127.0.0.1:5555";
    const char * pubProtocol = "tcp://127.0.0.1:5556";

    mySocket.bind(reqProtocol);
    pubSocket.bind(pubProtocol);

    if(!pubSocket.connected() || !mySocket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }
    
    int iteration = 0;

    //create a sentinel message to publish at the end of each loop
    json end = {
      {"id", -1},
      {"server iteration", 0},
    };


    json serverData = {
      {"id", 0},
      {"nextId", 1},
      {"iteration", 0},
    };



    while(true) {
      serverData["iteration"] = iteration++;
      int i = 0;
      do{
        // std::cout << "sending" << std::endl;
        mySocket.send(zmq::message_t(serverData.dump().c_str(), serverData.dump().length()), zmq::send_flags::none);
        // std::cout << "sent" << std::endl;

        zmq::message_t req;
        zmq::recv_result_t status = mySocket.recv(req, zmq::recv_flags::none);
        json response = json::parse(req.to_string());
        std::cout << response.dump().c_str() << std::endl;
        
        if(response["id"] == serverData["nextId"]) {
          clientCount++;
          nextId++;
          serverData["nextId"] = nextId;
        } 
        if(response["iteration"] == -1) {
          clientCount--;
        } else {
          pubSocket.send(req, zmq::send_flags::none);
        }

      } while(++i < clientCount);
      sleep(1);
      zmq::message_t sentinel(serverData.dump().c_str(), serverData.dump().length());
      pubSocket.send(sentinel, zmq::send_flags::none);
      }

    //close whats gotta close and exit
    pubSocket.close();
    mySocket.close();
    context.close();
    return 0;
}