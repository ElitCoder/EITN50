#include "NetworkCommunication.h"
#include "Cryptography.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    Cryptography::initialize();
    
    bool server = true;
    
    if(argc > 1) {
        server = false;
    }
    
    NetworkCommunication networkCommunication(20000, server);
    
    if(server) {
        networkCommunication.runServer();
    } else {
        networkCommunication.runClient();
    }
    
    /*
    string encrypted = encrypt("hej magnus");
    string decrypted = decrypt(encrypted);
    
    cout << "Encrypted text: " << encrypted << endl;
    cout << "Decrypted text: " << decrypted << endl;
    */
    
    return 0;
}