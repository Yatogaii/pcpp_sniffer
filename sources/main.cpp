#include <PcapLiveDevice.h>
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
#include "sockets.h"
#include <HttpLayer.h>
#include <ostream>
#include <vector>

/// 数据包处理函数
void printPacketInfo(const pcpp::Packet& packet) {
    pcpp::IPv4Layer* ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::IPv6Layer* ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();
    pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    pcpp::UdpLayer* udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
    pcpp::DnsLayer* dnsLayer = packet.getLayerOfType<pcpp::DnsLayer>();
    pcpp::HttpRequestLayer* httReqLayer = packet.getLayerOfType<pcpp::HttpRequestLayer>() ;
    pcpp::HttpResponseLayer* httRspLayer = packet.getLayerOfType<pcpp::HttpResponseLayer>() ;


    // Print basic IP/TCP/UDP information
    if (ipLayer != nullptr) {
        std::cout << "IP packet: " << ipLayer->getSrcIPAddress() << " -> " << ipLayer->getDstIPAddress();
        if (tcpLayer != nullptr) {
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

/// 获得当前设备的所有网卡，返回给主程序
/// 获得的网卡经过用户选择后再确定具体抓包流量来自哪一个网卡
std::vector<pcpp::PcapLiveDevice*> getLiveDevices(){
    return pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
}

int main() {
    // Step0: 初始化Socket，与前端通过socket通信
    // 基本流程：
    //   监听-收到连接-发送网卡列表-选择网卡-开始抓包
    Socket serverSocket;
    if (!serverSocket.Create()) {
        std::cerr << "Error creating server socket." << std::endl;
        return 1;
    }
    if (!serverSocket.Bind("127.0.0.1", 28080)) {
        std::cerr << "Error binding to address." << std::endl;
        return 1;
    }
    if (!serverSocket.Listen()) {
        std::cerr << "Error listening for connections." << std::endl;
        return 1;
    }

    // Step1: 获取网卡设备列表
    std::vector<pcpp::PcapLiveDevice*> devLists = getLiveDevices();
    /// TEST: Done
    ///for (auto each : devLists) {
    ///    std::cout << each->getName() << std::endl
    ///}
    ///return 0;


    // Step2: 选择设备
    // TODO

    // Step3: 开个子线程开始抓包，主线程接受进一步的消息
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp("172.24.27.33");
    if (!dev) {
        std::cerr << "Cannot find specific device!" << std::endl;
        return 1;
    }

    if (!dev->open()) {
        std::cerr << "Cannot open device!" << std::endl;
        return 1;
    }

    dev->startCapture(onPacketArrives, nullptr);

    // Step4: 主线程接受进一步的消息，阻塞
    getchar();

    // Step5: 退出程序，或者重置状态后等待下一次抓包
    // TODO
    dev->stopCapture();
    dev->close();

    return 0;
}
