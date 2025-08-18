// http protocol is system independent
// what differs is the system calls to talk to the network
// hence there are different libs for handling open/closed resources

// unix based systems use POSIX which works on principle of FDs(file descriptors) which returns 
// an int to represent an open socket/pipe

// windows uses WINSOCK which returns a value of type SOCKET(basically a pointer under the hood)

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <sstream>


#define PORT 8080
// binding == giving a socket an address, ip add & port no.

int main() {

    //init WINSOCK
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed!!\n";   //cerr is an errorstream to o/p errors, unlike cout it's unbuffered
        return 1;
    }   
    //WSAStartup func is called to initiate use of WS2_32.dll
    //basically sets up the compatible version of winsock for your sys to get started 


    //server side
    //socket creation

    //socket(adressFamily{ipv4, ipv6}, socketType{stream socket like TCP}, protocol)
    //protocol -> protocol for the chosen socket type, for TCP -> IPPROTO_TCP, UDP -> IPPROTO_UDP, 0 -> default acc to socketType
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == INVALID_SOCKET) {
        std::cerr << "socket creation failed!!\n";
        WSACleanup();
        return 1;
    }

    //socket definition

    sockaddr_in socketAddr;     //sockaddr_in is a ipv4 specific struct to store address
    socketAddr.sin_family = AF_INET;    //tells winsock it's a TCP based sock
    socketAddr.sin_port = htons(PORT);  //htons to conv to big-endian
    socketAddr.sin_addr.s_addr = INADDR_ANY;    //sin_addr -> IP addr to bind to
    //INADDR_ANY -> listen on all local interfaces, anyone connected to same network can access the server, basically 0.0.0.0:8080

    if(bind(server, (sockaddr*)&socketAddr, sizeof(socketAddr)) == SOCKET_ERROR) {
        std::cerr << "bind failed!!\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }

    //listen(server, backlog{max connections in queue}) SOMAXCONN is provided by winsock letting the OS decide upon the number
    if(listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << "\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }

    std::cout << "server is running on port " << PORT << "\n";


    //client side
    SOCKET client;
    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);

    while((client = accept(server, (sockaddr*)&clientAddr, &clientSize)) != INVALID_SOCKET) {
        char buffer[4096];  
        int bytesReceived = recv(client, buffer, sizeof(buffer), 0);

        if(bytesReceived == SOCKET_ERROR) {
            std::cerr << "receive failed: " << WSAGetLastError() << "\n";
            closesocket(client);
            continue;
        }

        if(bytesReceived > 0) {
            std::string request(buffer, bytesReceived);
            std::cout << "request:\n" << request << "\n";

            //check method
            std::string method = request.substr(0, request.find(" ")); 

            if(method == "POST") {
                //find content len
                size_t pos = request.find("Content-Length:");

                int contentLength = 0;
                if(pos != std::string::npos) {
                    std::istringstream iss(request.substr(pos+15));
                    iss >> contentLength;
                }
                //find "Content-Length:" in req body, after that there's a no. representing the length
                //"Content-Length:".size() = 15, we use iss to append the no. therafter in contentLength

                size_t bodyStart = request.find("\r\n\r\n");
                std::string body;
                if(bodyStart != std::string::npos) {
                    body = request.substr(bodyStart+4, contentLength);
                }

                std::cout << "POST body: " << body << "\n";

                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    "Received POST data: " + body;

                send(client, response.c_str(), response.size(), 0);
            }


            else {
                std::string response = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    "heyo!";

                send(client, response.c_str(), response.size(), 0); //c_str is used to conv string to an arr of chars in the string
            }
        }
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}
