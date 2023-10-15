#include "packets.h"
#include <cstring>
#include <arpa/inet.h>

const size_t DataPack::HEADER_SIZE;

DataPack::DataPack() = default;

DataPack::DataPack(const std::string& data) {
    size_t totalSize = HEADER_SIZE + data.size();
    buffer.resize(totalSize);

    uint32_t dataSize = htonl(data.size());  // convert to network byte order
    std::memcpy(buffer.data(), &dataSize, HEADER_SIZE);
    std::memcpy(buffer.data() + HEADER_SIZE, data.data(), data.size());
}

char* DataPack::data() {
    return buffer.data();
}

size_t DataPack::size() const {
    return buffer.size();
}

std::string DataPack::unpackData() const {
    if (buffer.size() <= HEADER_SIZE) return "";
    uint32_t dataSize;
    std::memcpy(&dataSize, buffer.data(), HEADER_SIZE);
    dataSize = ntohl(dataSize);  // convert to host byte order
    return std::string(buffer.data() + HEADER_SIZE, dataSize);
}

void DataPack::append(const char* data, size_t len) {
    buffer.insert(buffer.end(), data, data + len);
}
