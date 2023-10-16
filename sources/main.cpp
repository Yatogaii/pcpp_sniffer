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

std::vector<pcpp::ProtocolType> getProtocolStackFromTop(const pcpp::Packet& packet) {
    std::vector<pcpp::ProtocolType> protocols;

    pcpp::Layer* currLayer = packet.getFirstLayer();
    while (currLayer != nullptr) {
        protocols.push_back(currLayer->getProtocol());
        currLayer = currLayer->getNextLayer();
    }

    // 反转protocols向量，这样顶层的协议就位于向量的开头
    std::reverse(protocols.begin(), protocols.end());

    return protocols;
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
