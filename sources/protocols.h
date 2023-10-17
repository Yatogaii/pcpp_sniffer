#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

#include <DnsLayer.h>
#include <HttpLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <Packet.h>
#include <PcapFileDevice.h>
#include <PcapLiveDeviceList.h>
#include <ProtocolType.h>
#include <TcpLayer.h>
#include <UdpLayer.h>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <vector>

struct LayerInfo {
    std::string protocolName;
    // 存放每个协议的特定字段
    std::map<std::string, std::string> fields;
    // payload
    unsigned char *payload;
};

std::vector<pcpp::ProtocolType> getProtocolStack(const pcpp::Packet &packet);
void parseLayer(pcpp::Layer *layer, std::vector<LayerInfo> &layersInfo);

#endif
