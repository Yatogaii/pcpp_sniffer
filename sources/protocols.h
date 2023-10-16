#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <TcpLayer.h>
#include <UdpLayer.h>
#include <DnsLayer.h>
#include <PcapFileDevice.h>
#include <Packet.h>
#include <PcapLiveDeviceList.h>
#include <ProtocolType.h>
#include <HttpLayer.h>
#include <ostream>
#include <vector>
#include <string>

struct LayerInfo {
    std::string protocolName;
	// 存放每个协议的特定字段
    std::map<std::string, std::string> fields;
};


std::vector<pcpp::ProtocolType> getProtocolStackFromTop(const pcpp::Packet& packet);
void parseLayer(pcpp::Layer* layer, std::vector<LayerInfo>& layersInfo);

#endif
