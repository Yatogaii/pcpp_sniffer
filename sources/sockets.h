#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Socket {
public:
    Socket();
    ~Socket();

    bool Create();
    bool Bind(const std::string& address, int port);
    bool Listen(int backlog = 5);
    bool Accept(Socket& newSocket);
    bool Connect(const std::string& address, int port);
    bool Send(const std::string& data);
    bool Receive(std::string& data);
    void Close();

    // 添加移动构造函数和移动赋值运算符
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);

private:
    int m_socket;
};

#endif
