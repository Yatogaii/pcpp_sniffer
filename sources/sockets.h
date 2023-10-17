#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "packets.h"

class Socket {
  public:
    Socket();
    ~Socket();

    bool Create();
    bool Bind(const std::string &address, int port);
    bool Listen(int backlog = 5);
    bool Accept(Socket &newSocket);
    bool Connect(const std::string &address, int port);
    bool Send(const std::string &data);
    bool Send(const DataPack &data);
    bool Receive(std::string &data);
    void Close();

    // 添加移动构造函数和移动赋值运算符
    Socket(Socket &&other);
    Socket &operator=(Socket &&other);

  private:
    int m_socket;
};

#include <condition_variable>
#include <mutex>
#include <queue>

// 同步队列，子线程处理数据，交给主线程发送。
template <typename T> class SyncQueue {
  public:
    void push(const T &item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cond.notify_one(); // 通知等待线程有新的元素可以处理
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock,
                    [this] { return !m_queue.empty(); }); // 等待直到队列非空
        T val = m_queue.front();
        m_queue.pop();
        return val;
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

  private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif
