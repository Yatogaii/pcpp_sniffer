#ifndef  _PACKETS_H_
#define  _PACKETS_H_

#include <iostream>
#include <vector>
#include <string>

class DataPack {
private:
    static const size_t HEADER_SIZE = sizeof(int);  // 4 bytes for data length
    std::vector<char> buffer;

public:
    DataPack();
    DataPack(const std::string& data);
    DataPack(const std::vector<std::string>& data);

    const char* data() const;
    size_t size() const;
    std::string unpackData() const;
    void append(const char* data, size_t len);
};

#endif
