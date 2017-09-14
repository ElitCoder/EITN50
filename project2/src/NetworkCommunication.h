#ifndef NETWORK_COMMUNICATION_H
#define NETWORK_COMMUNICATION_H

#include <sys/socket.h>

class NetworkCommunication {
public:
    NetworkCommunication(const unsigned short port, const bool server);
    
    void runServer();
    void runClient();
    
private:
    int m_masterSocket;    
    sockaddr_storage m_address;
};

#endif