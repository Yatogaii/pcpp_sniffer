#ifndef  _PACKETS_H_
#define  _PACKETS_H_

#include <iostream>
#include <vector>
#include <string>

class DataPack {
private:
    // Soket包头长度固定为2个int, 一个int是数据包类型，另一个int是数据包长度
    static const size_t HEADER_SIZE = sizeof(int) * 2;
    const int dataType;
    std::vector<char> buffer;

public:
    static const int kDataTypeDevList = 0;
    static const int kDataTypeLayerInfos = 1;
    DataPack():dataType(0){};
    DataPack(const std::string& data, int);
    DataPack(const std::vector<std::string>& data, int);

    const char* data() const;
    size_t size() const;
    std::string unpackData() const;
    void append(const char* data, size_t len);
};

#endif
