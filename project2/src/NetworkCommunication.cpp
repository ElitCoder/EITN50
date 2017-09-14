#include "NetworkCommunication.h"
#include "Cryptography.h"

#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>

using namespace std;

NetworkCommunication::NetworkCommunication(const unsigned short port, const bool server) {
    signal(SIGPIPE, SIG_IGN);
    
    m_masterSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(m_masterSocket < 0) {
        cout << "ERROR: socket() failed\n";
        
        return;
    }
    
    if(server) {
        sockaddr_in socketInformation;
        socketInformation.sin_family = AF_INET;
        socketInformation.sin_addr.s_addr = INADDR_ANY;
        socketInformation.sin_port = htons(port);
        
        int on = 1;
        
        if(setsockopt(m_masterSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&on), sizeof(on)) < 0) {
            cout << "ERROR: not reusable address\n";
            
            return;
        }
        
        if(bind(m_masterSocket, reinterpret_cast<sockaddr*>(&socketInformation), sizeof(socketInformation)) < 0) {
            cout << "ERROR: bind() failed\n";
            
            close(m_masterSocket);
            return;
        }
        
        size_t socketInformationSize = sizeof(socketInformation);

        if(getsockname(m_masterSocket, reinterpret_cast<sockaddr*>(&socketInformation), reinterpret_cast<socklen_t*>(&socketInformationSize)) < 0) {
            cout << "ERROR: could not get address information\n";
            
            close(m_masterSocket);
            return;
        }
    } else {
        addrinfo *result_list = NULL;
        addrinfo hints = {};
                
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        
        if(getaddrinfo("127.0.0.1", "20000", &hints, &result_list) == 0) {
            memcpy(&m_address, result_list->ai_addr, result_list->ai_addrlen);
            freeaddrinfo(result_list);
        } else {
            cout << "ERROR: could not get address information\n";
        }
        
        //socketInformation.sin_family = AF_INET;
        //socketInformation.sin_socktype = SOCK_DGRAM;
    }
}

void NetworkCommunication::runServer() {
    unsigned char *buffer = new unsigned char[8192];
    
    while(true) {
        sockaddr_in clientAddress;
        unsigned int clientAddressSize = sizeof(clientAddress);
        
        int received = recvfrom(m_masterSocket, buffer, 8192, 0, (sockaddr*)&clientAddress, &clientAddressSize);
        
        if(received <= 0) {
            cout << "Exiting server\n";
            
            break;
        }
        
        cout << "Encrypted: " << buffer << endl;
        
        string message((char*)buffer, received);
        string decryptedText;
        
        Cryptography::decrypt(message, decryptedText);
        
        cout << "Decrypted: " << decryptedText << endl;
    }
    
    delete[] buffer;
}

void NetworkCommunication::runClient() {
    string message;
    Cryptography::encrypt("hej magnus", message);
    
    int sent = sendto(m_masterSocket, message.c_str(), message.length(), 0, (sockaddr*)&m_address, sizeof(m_address));
    
    if(sent > 0) {
        cout << "Sent " << sent << " bytes\n";
    }
    
    /*
    string message = "hej magnus";
    
    int sent = sendto(m_masterSocket, message.c_str(), message.length(), 0, (sockaddr*)&m_address, sizeof(m_address));
    
    if(sent > 0) {
        cout << "Sent " << sent << " bytes\n";
    }
    */
    
    // CLIENT
}