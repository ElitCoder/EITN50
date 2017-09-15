#ifndef NETWORK_COMMUNICATION_H
#define NETWORK_COMMUNICATION_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class NetworkCommunication {
public:
    NetworkCommunication(const unsigned short port, const bool server);
    ~NetworkCommunication();
    
    NetworkCommunication(const NetworkCommunication &networkCommunication) = delete;
    NetworkCommunication(NetworkCommunication &&networkCommunication) = delete;
    
    NetworkCommunication& operator=(const NetworkCommunication &networkCommunication) = delete;
    NetworkCommunication& operator=(NetworkCommunication &&networkCommunication) = delete;
    
    void runServer();
    void runClient();
    
    int handshake(const bool start);
    
    int sendPacket(int fd, sockaddr_storage *address, const std::string &message, const bool encrypt = true);
    std::string receivePacket(int fd, unsigned char **buffer, int length, const bool decrypt = true);
    
private:
    int m_masterSocket;    
    unsigned char *m_buffer;
    sockaddr_storage m_address;
    
    sockaddr_in m_clientAddress;
};

#endif