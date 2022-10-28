Three libraries were apt installed for this project:
libzmq3-dev, libzmqpp-dev, and nlohmann-json3-dev

All parts of the assignment are in one directory, and all the executables can be compiled by running "make",
./main will run a singleplayer instance that fully demonstrates part 1.

./server will launch an sfml window which also fully demonstrates part 1, 
but can also be connected to by running ./client

After starting a server you can connect and disconnect clients as much as you want,
but closing the server will break the clients.

Starting a client and then launching the server works sometimes, but can be somewhat unpredictable.
