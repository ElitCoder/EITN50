#include "NetworkCommunication.h"
#include "Cryptography.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv) {    
    bool server = true;
    
    if(argc > 1) {
        server = false;
    }
    
    if(argv[0]) {
    }
    
    NetworkCommunication networkCommunication(20000, server);
    
    if(server) {
        networkCommunication.runServer();
    } else {
        networkCommunication.runClient();
    }
    
    return 0;
}