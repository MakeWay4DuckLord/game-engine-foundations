Three libraries were apt installed for this project:
libzmq3-dev, libzmqpp-dev, and nlohmann-json3-dev

All parts of the assignment are in one directory.
All the executables can be compiled by running "make",
or compiled individually with "make main", "make server", and "make client".

./main will run a singleplayer instance that fully demonstrates part 1.

./server will launch an sfml window which demonstrates part 1 and 2, 
./client will launch create another window with its own character and connect to the server.

After starting a server you can connect and disconnect clients as much as you want,
but closing the server will break the clients.

Pressing the R key while the server window is in focus will raise a SPAWN event on all clients