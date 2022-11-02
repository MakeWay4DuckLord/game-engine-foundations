#include <zmq.hpp>
#include <string.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {

    //connect to the server with a req socket
    zmq::context_t context(1);
    const char * repProtocol = "tcp://127.0.0.1:5555";
    const char * subProtocol = "tcp://127.0.0.1:5556";
    zmq::socket_t mySocket (context, zmq::socket_type::rep);
    zmq::socket_t subSocket (context, zmq::socket_type::sub);
    mySocket.connect(repProtocol);
    subSocket.connect(subProtocol);

    subSocket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   if(!subSocket.connected()) {
      std::cout << "socket cannot be bound??" << std::endl;
      return 1;
    }

    //create client state
    json client = {
        {"id", -1},
        {"iteration", 0},
    };

    //go ahead and make shutdown message:
    
    for(int i = 0; i < 20; i++) {
        // std::cout << "wating for server" << std::endl;
        zmq::message_t reply;
        mySocket.recv(reply, zmq::recv_flags::none);

        json update = json::parse(reply.to_string());

        // std::cout << update.dump().c_str() << std::endl;

        client["iteration"] = i;

        int iteration = update["iteration"];

        if(client["id"] == -1) {
            client["id"] = update["nextId"];
        }


        std::string msgStr = client.dump();
        zmq::message_t msg(msgStr.c_str(), msgStr.length());
        mySocket.send(msg, zmq::send_flags::none);

        //dont need to do anything with this reply
        // mySocket.close();

        //now just print out every message published to subscriber socket
        bool reading = true;
        while(reading) {
            // std::cout << "reading" << std::endl;
            zmq::message_t data;
            subSocket.recv(data, zmq::recv_flags::none);
            // std::cout << "parsing: " << data.to_string() << std::endl;
            if(data.to_string().length() > 0) {
                json update = json::parse(data.to_string());
                if(update["id"] == 0) {
                    // std::cout << "no longer reading" << std::endl;
                    std::cout << update["iteration"] << std::endl;
                    if(update["iteration"] == iteration) {
                        reading = false;
                    }
                } else {
                    std::cout << "Client " << update["id"] << ": Iteration " << update["iteration"] << std::endl;
                }
            }
        }
    }
    zmq::message_t foo;
    mySocket.recv(foo, zmq::recv_flags::none);
    client["iteration"] = -1;
    zmq::message_t shutdown(client.dump().c_str(), client.dump().length());
    mySocket.send(shutdown, zmq::send_flags::none);

    mySocket.close();
    subSocket.close();
    context.close();    

    return 0;
}