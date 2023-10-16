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


/// 从下至上的获取一个 Packet 的协议栈
/// eg. Ethernet->IPv4->Tcp->Http
std::vector<pcpp::ProtocolType> getProtocolStackFromTop(const pcpp::Packet& packet) {
    std::vector<pcpp::ProtocolType> protocols;

    pcpp::Layer* currLayer = packet.getFirstLayer();
    while (currLayer != nullptr) {
        protocols.push_back(currLayer->getProtocol());
        currLayer = currLayer->getNextLayer();
    }

    return protocols;
}

/// parseLayer 函数，处理传入的数据包
/// 根据协议栈来拼接 map<string,string> 类型的数据
/// TODO 需要确定各个协议都需要存入什么类型的数据
void parseLayer(pcpp::Layer* layer, std::vector<LayerInfo>& layersInfo) {
    while (layer) {
        LayerInfo currLayerInfo;
        currLayerInfo.protocolName = layer->getProtocolName();

        switch (layer->getProtocol()) {
            case pcpp::Ethernet:
                // 处理Ethernet层字段
                break;
            case pcpp::IPv4:
            case pcpp::IPv6:
                {
                    auto ipLayer = dynamic_cast<pcpp::IPLayer*>(layer);
                    currLayerInfo.fields["Source IP"] = ipLayer->getSrcIPAddress().toString();
                    currLayerInfo.fields["Destination IP"] = ipLayer->getDstIPAddress().toString();
                }
                break;
            case pcpp::ARP:
                // 处理ARP层字段
                break;
            case pcpp::ICMP:
                // 处理ICMP层字段
                break;
            case pcpp::TCP:
                {
                    auto tcpLayer = dynamic_cast<pcpp::TcpLayer*>(layer);
                    currLayerInfo.fields["Source Port"] = std::to_string(tcpLayer->getTcpHeader()->portSrc);
                    currLayerInfo.fields["Destination Port"] = std::to_string(tcpLayer->getTcpHeader()->portDst);
                }
                break;
            case pcpp::UDP:
                {
                    auto udpLayer = dynamic_cast<pcpp::UdpLayer*>(layer);
                    currLayerInfo.fields["Source Port"] = std::to_string(udpLayer->getUdpHeader()->portSrc);
                    currLayerInfo.fields["Destination Port"] = std::to_string(udpLayer->getUdpHeader()->portDst);
                }
                break;
            case pcpp::HTTP:
                {
                    auto httpLayer = dynamic_cast<pcpp::HttpLayer*>(layer);
                    currLayerInfo.fields["HTTP Method"] = httpLayer->getFirstLine()->getMethod();
                    currLayerInfo.fields["HTTP URI"] = httpLayer->getFirstLine()->getUri();
                }
                break;
            // ... 处理其他应用层协议
        }

        layersInfo.push_back(currLayerInfo);

        // Move to the next layer
        layer = layer->getNextLayer();
    }
}


/// 数据包处理函数
void printPacketInfo(const pcpp::Packet& packet) {
    // Step1: 获取 Packet 的协议栈
    std::vector<pcpp::ProtocolType> protocols = getProtocolStackFromTop(packet);

    pcpp::IPv4Layer* ipv4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::IPv6Layer* ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();
    pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    pcpp::UdpLayer* udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
    pcpp::DnsLayer* dnsLayer = packet.getLayerOfType<pcpp::DnsLayer>();
    pcpp::HttpRequestLayer* httReqLayer = packet.getLayerOfType<pcpp::HttpRequestLayer>() ;
    pcpp::HttpResponseLayer* httRspLayer = packet.getLayerOfType<pcpp::HttpResponseLayer>() ;

    // Print basic IP/TCP/UDP information
    if (ipv4Layer != nullptr) {
        std::cout << "IP packet: " << ipv4Layer->getSrcIPAddress() << " -> " << ipv4Layer->getDstIPAddress();
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
    Socket clientSocket;
    if (!serverSocket.Accept(clientSocket)) {
        //TODO
    }

    // Step1: 获取网卡设备列表
    std::vector<pcpp::PcapLiveDevice*> devLists = getLiveDevices();
    /// TEST: Done
    ///for (auto each : devLists) {
    ///    std::cout << each->getName() << std::endl
    ///}
    ///return 0;


    // Step2: 选择设备
    // Step1: 拼接列表字符串
    std::vector<std::string> devDatas;
    for(auto each : devLists){
        devDatas.push_back(each->getName());
        devDatas.push_back("!@#");
    }
    // Step2.2: 发送给前端供用户选择
    clientSocket.Send(DataPack(devDatas));

    // Step2.3: 接受前端返回的数据
    std::string recvData;
    clientSocket.Receive(recvData);
    // 判断数据是否合理
    // TODO 可能需要对recvDatas 进行提取
    if(std::find(devDatas.begin(),devDatas.end(),recvData)==devDatas.end()){
        // TODO 接受到的数据不在网卡列表里，错误处理
    }

    // Step3: 开个子线程开始抓包，主线程接受进一步的消息
    // pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(recvData.c_str());
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

    // Step4: 主线程接受进一步的消息
    // TODO
    getchar();

    // Step5: 退出程序，或者重置状态后等待下一次抓包
    // TODO
    dev->stopCapture();
    dev->close();

    return 0;
}
