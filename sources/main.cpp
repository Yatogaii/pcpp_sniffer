#include <iostream>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <TcpLayer.h>
#include <UdpLayer.h>
#include <DnsLayer.h>
#include <PcapFileDevice.h>
#include <Packet.h>
#include <PcapLiveDeviceList.h>
#include <ProtocolType.h>
#include "packets.h"
#include <HttpLayer.h>

/// 数据包处理函数
void printPacketInfo(const pcpp::Packet& packet) {
    pcpp::IPv4Layer* ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::IPv6Layer* ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();
    pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    pcpp::UdpLayer* udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
    pcpp::DnsLayer* dnsLayer = packet.getLayerOfType<pcpp::DnsLayer>();
    pcpp::HttpRequestLayer httReqLayer = packet.getLayerOfType<pcpp::HttpRequestLayer>() ;
    pcpp::HttpResponseLayer httRspLayer = packet.getLayerOfType<pcpp::HttpResponseLayer>() ;


    // Print basic IP/TCP/UDP information
    if (ipLayer != nullptr) {
        std::cout << "IP packet: " << ipLayer->getSrcIPAddress() << " -> " << ipLayer->getDstIPAddress();
        if (tcpLayer != nullptr) {
            std::cout << " TCP " << tcpLayer->getSrcPort() << " -> " << tcpLayer->getDstPort();
        } else if (udpLayer != nullptr) {
            std::cout << " UDP " << udpLayer->getSrcPort() << " -> " << udpLayer->getDstPort();
        }
        std::cout << std::endl;
    }

    // Print DNS information
    if (dnsLayer != nullptr) {
        std::cout << "DNS query: " << dnsLayer->getQueryCount() << " queries" << std::endl;
    }
}

void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
    pcpp::Packet parsedPacket(packet);
    printPacketInfo(parsedPacket);
}

int main() {
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp("172.24.27.33");
    if (!dev) {
        std::cerr << "Cannot find default device!" << std::endl;
        return 1;
    }

    if (!dev->open()) {
        std::cerr << "Cannot open device!" << std::endl;
        return 1;
    }

    dev->startCapture(onPacketArrives, nullptr);

    getchar();

    dev->stopCapture();
    dev->close();

    return 0;
}
