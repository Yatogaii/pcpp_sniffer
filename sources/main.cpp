#include <PcapLiveDevice.h>
#include <iostream>
#include <ostream>
#include <vector>

#include "packets.h"
#include "sockets.h"
#include "protocols.h"

/// 数据包处理示例函数
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

/// 数据包处理函数
void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
    // Step0: 把 Raw 解析成 Packet
    pcpp::Packet parsedPacket(packet);

    // TODO 是否需要同步队列进行多线程处理？

    // Step1: 获取 Packet 的协议栈
    std::vector<pcpp::ProtocolType> protocols = getProtocolStackFromTop(packet);

    // Step2: 处理所有协议，每个协议都读取相关的信息
    // TODO 这一步到底需要存放哪些信息不好确定
    // printPacketInfo(parsedPacket);
    std::vector<LayerInfo> packetLayerInfos;
    parseLayer(parsedPacket.getFirstLayer(), packetLayerInfos);

    // Step3: 把处理到的数据包发给前端进行解析
    // 需要确定数据包拼接的形式
    
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
    clientSocket.Send(DataPack(devDatas, DataPack::kDataTypeDevList));

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
