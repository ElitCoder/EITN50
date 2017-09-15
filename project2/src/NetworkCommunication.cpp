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
#include <crypto++/dh.h>
#include <crypto++/osrng.h>
#include <crypto++/nbtheory.h>
#include <random>

using namespace std;

uint64_t pow64(uint32_t b, uint32_t e) {
    if(e == 0)
        return 1;
        
    return b * pow(b, e - 1);
}

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
    }
    
    m_buffer = new unsigned char[8192];
}

NetworkCommunication::~NetworkCommunication() {
    delete[] m_buffer;
}

void NetworkCommunication::runServer() {
    Cryptography::initialize(handshake(false));
    
    auto received = receivePacket(m_masterSocket, &m_buffer, 8192);
    
    cout << "<MAIN> Received packet: " << received << endl;
    cout << "<MAIN> Length: " << received.length() << endl;
    
    cout << "<MAIN> In hex: ";
    
    for(unsigned int i = 0; i != received.length(); ++i)
        printf("%x ", received.at(i));
        
    cout << endl;
}

int NetworkCommunication::sendPacket(int fd, sockaddr_storage *address, const string &message, const bool encrypt) {
    string encrypted;
    
    if(encrypt) {
        encrypted = Cryptography::encrypt(message);
        encrypted += Cryptography::createHMAC(encrypted);
    } else {
        encrypted = message;
    }
    
    int sent = sendto(fd, encrypted.c_str(), encrypted.length(), 0, reinterpret_cast<sockaddr*>(address), sizeof(*address));
    
    if(sent <= 0) {
        cout << "<SP> Length: " << encrypted.length() << endl;
        cout << "<SP> Lost connection\n";
    }
    
    return sent;
}

string NetworkCommunication::receivePacket(int fd, unsigned char **buffer, int length, const bool decrypt) {
    sockaddr_in clientAddress;
    unsigned int clientAddressSize = sizeof(clientAddress);
    
    int received = recvfrom(fd, *buffer, length, 0, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
    
    if(received <= 0) {
        cout << "<RP> Lost connection\n";
        
        return string();
    }
    
    string encrypted = string(reinterpret_cast<char*>(*buffer), received);
    string decrypted;
    
    if(decrypt) {
        string hmac = encrypted.substr(encrypted.length() - 32);
        encrypted = encrypted.substr(0, encrypted.length() - 32);
        
        bool valid = Cryptography::checkHMAC(hmac, encrypted);
        
        if(valid)
            decrypted = Cryptography::decrypt(encrypted);
        else
            cout << "<RP> HMAC does not match\n";
    } else {
        decrypted = encrypted;
    }
        
    m_clientAddress = clientAddress;
    
    return decrypted;
}

int NetworkCommunication::handshake(const bool start) {
    cout << "<DH> Performing Diffie-Hellman key exchange\n";
    
    unsigned char p, g;
    int S, s, a;
    
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(5, 10);
    
    if(start) {
        CryptoPP::PrimeAndGenerator primeAndGenerator;
        CryptoPP::AutoSeededRandomPool randomPool;
        primeAndGenerator.Generate(1, randomPool, 4, 3);
        
        p = primeAndGenerator.Prime().ConvertToLong();
        g = primeAndGenerator.Generator().ConvertToLong();
        
        cout << "<DH> p: " << static_cast<int>(p) << ", g: " << static_cast<int>(g) << endl;
        
        array<unsigned char, 2> message = {p, g};
        sendPacket(m_masterSocket, reinterpret_cast<sockaddr_storage*>(&m_address), string(message.data(), message.data() + 2), false);
    } else {
        auto parameters = receivePacket(m_masterSocket, &m_buffer, 8192, false);
        p = parameters.at(0);
        g = parameters.at(1);
        
        cout << "<DH> p: " << static_cast<int>(p) << ", g: " << static_cast<int>(g) << endl;
        
        a = dist(mt);
        auto A = pow64(g, a) % p;
        
        cout << "<DH> A: " << A << ", a: " << a << endl;
        
        sendPacket(m_masterSocket, reinterpret_cast<sockaddr_storage*>(&m_clientAddress), string(1, static_cast<char>(A)), false);
    }
    
    if(start) {
        int A = receivePacket(m_masterSocket, &m_buffer, 8192, false).at(0);
        int b = dist(mt);
        auto B = pow64(g, b) % p;
        
        cout << "<DH> B: " << B << ", b: " << b << endl;
        cout << "<DH> A: " << A << endl;
        
        S = A;
        s = b;
        
        sendPacket(m_masterSocket, reinterpret_cast<sockaddr_storage*>(&m_address), string(1, static_cast<char>(B)), false);
    } else {
        int B = receivePacket(m_masterSocket, &m_buffer, 8192, false).at(0);
        
        cout << "<DH> B: " << B << endl;
        
        S = B;
        s = a;
    }
    
    auto shared = pow64(S, s) % p;
    cout << "<DH> s: " << shared << endl;
    cout << "<DH> Finished Diffie-Hellman\n";
    
    return shared;
}

void NetworkCommunication::runClient() {
    Cryptography::initialize(handshake(true));
    
    sendPacket(m_masterSocket, reinterpret_cast<sockaddr_storage*>(&m_address), "this is a message");
}