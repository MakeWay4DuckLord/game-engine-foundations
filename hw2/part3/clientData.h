//clientData struct to be sent in messages
struct clientData {
    int id;
    int iteration;
};

#define DATA_SIZE sizeof(clientData) / 4