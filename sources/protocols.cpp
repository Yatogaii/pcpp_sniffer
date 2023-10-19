#include "protocols.h"
#include <Device.h>
#include <Layer.h>
#include <ProtocolType.h>
#include <bits/types/struct_timespec.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

// 把rawdata转化为16进制，要不然会报错
std::string toHex(const uint8_t *data, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(data[i]);
    }
    return ss.str();
}

/// 从下至上的获取一个 Packet 的协议栈
/// eg. Ethernet->IPv4->Tcp->Http
std::vector<pcpp::ProtocolType> getProtocolStack(const pcpp::Packet &packet) {
    std::vector<pcpp::ProtocolType> protocols;

    pcpp::Layer *currLayer = packet.getFirstLayer();
    while (currLayer != nullptr) {
        protocols.push_back(currLayer->getProtocol());
        currLayer = currLayer->getNextLayer();
    }

    return protocols;
}

/// parseLayer 函数，处理传入的数据包
/// 根据协议栈来拼接 map<string,string> 类型的数据
/// TODO 需要确定各个协议都需要存入什么类型的数据
//; void parseLayer(pcpp::Layer *layer, std::vector<LayerInfo> &layersInfo) {
//;   while (layer) {
//;     LayerInfo currLayerInfo;
//;     // TODO 需要获取协议名称
//;     // currLayerInfo.protocolName = layer->getProtocolName();
//;     currLayerInfo.protocolName = "protos";
//;
//;     switch (layer->getProtocol()) {
//;     case pcpp::Ethernet:
//;       break;
//;     case pcpp::IPv4:
//;     case pcpp::IPv6: {
//;       auto ipLayer = dynamic_cast<pcpp::IPLayer *>(layer);
//;       currLayerInfo.fields["Source IP"] =
// ipLayer->getSrcIPAddress().toString(); ; currLayerInfo.fields["Destination
// IP"] = ;           ipLayer->getDstIPAddress().toString(); ;     } break; ;
// case pcpp::ARP: ;       break; ;     case pcpp::ICMP: ;       break; ; case
// pcpp::TCP: { ;       auto tcpLayer = dynamic_cast<pcpp::TcpLayer *>(layer); ;
// currLayerInfo.fields["Source Port"] = ;
// std::to_string(tcpLayer->getTcpHeader()->portSrc); ;
// currLayerInfo.fields["Destination Port"] = ;
// std::to_string(tcpLayer->getTcpHeader()->portDst); ;     } break; ;     case
// pcpp::UDP: { ;       auto udpLayer = dynamic_cast<pcpp::UdpLayer *>(layer); ;
// currLayerInfo.fields["Source Port"] = ;
// std::to_string(udpLayer->getUdpHeader()->portSrc); ;
// currLayerInfo.fields["Destination Port"] = ;
// std::to_string(udpLayer->getUdpHeader()->portDst); ;     } break; ;     case
// pcpp::HTTPResponse: { ;       auto httpRspsLayer =
// dynamic_cast<pcpp::HttpResponseLayer *>(layer); ;       //
// currLayerInfo.fields["HTTP Method"] = ;       //
// httpLayer->getFirstLine()->getMethod(); ;       // currLayerInfo.fields["HTTP
// URI"] = ;       // httpLayer->getFirstLine()->getUri(); ;     } ;     case
// pcpp::HTTPRequest: { ;     } break; ;       // ... 处理其他应用层协议 ;     }
//;
//;     layersInfo.push_back(currLayerInfo);
//;
//;     // Move to the next layer
//;     layer = layer->getNextLayer();
//;   }
//; }

std::string getProtocolName(pcpp::ProtocolType protocol) {
    switch (protocol) {
    case pcpp::Ethernet:
        return "Ethernet";
    case pcpp::IPv4:
        return "IPv4";
    case pcpp::IPv6:
        return "IPv6";
    case pcpp::ARP:
        return "ARP";
    case pcpp::ICMP:
        return "ICMP";
    case pcpp::TCP:
        return "TCP";
    case pcpp::UDP:
        return "UDP";
    case pcpp::HTTPResponse:
        return "HTTP Response";
    case pcpp::HTTPRequest:
        return "HTTP Request";
    case pcpp::DNS:
        return "DNS";
    case pcpp::FTP:
        return "FTP";
    case pcpp::SSH:
        return "SSH";
    case pcpp::SSL:
        return "SSL";
    case pcpp::SIP:
        return "SIP";
    case pcpp::SDP:
        return "POP3";
    case pcpp::DHCP:
        return "DHCP";
    case pcpp::NTP:
        return "NTP";
    case pcpp::WakeOnLan:
        return "WakeOnLan";
    default:
        return "Unknown";
    }
}

// 1018 层级结构不合理，前端没的展示
// void parseLayer(pcpp::Layer *layer, pcpp::RawPacket *packet,
//                std::vector<LayerInfo> &layersInfo) {
//    // 存放最上的一个 layer ，因为每个包的payload只存放最上面一层协议的。
//    pcpp::Layer *lastLayer = layer;
//    while (layer) {
//        LayerInfo currLayerInfo;
//
//        // Set protocol name based on protocol type
//        // currLayerInfo.protocolName = getProtocolName(layer->getProtocol());
//        currLayerInfo.protocolName = getProtocolName(layer->getProtocol());
//
//        switch (layer->getProtocol()) {
//        case pcpp::Ethernet:
//            // No specific data extraction for Ethernet in this context
//            break;
//        case pcpp::IPv4:
//        case pcpp::IPv6: {
//            auto ipLayer = dynamic_cast<pcpp::IPLayer *>(layer);
//            currLayerInfo.fields["Source IP"] =
//                ipLayer->getSrcIPAddress().toString();
//            currLayerInfo.fields["Destination IP"] =
//                ipLayer->getDstIPAddress().toString();
//            break;
//        }
//        case pcpp::ARP:
//            // Extract ARP-specific fields if needed
//            break;
//        case pcpp::ICMP:
//            // Extract ICMP-specific fields if needed
//            break;
//        case pcpp::TCP: {
//            auto tcpLayer = dynamic_cast<pcpp::TcpLayer *>(layer);
//            currLayerInfo.fields["Source Port"] =
//                std::to_string(ntohs(tcpLayer->getTcpHeader()->portSrc));
//            currLayerInfo.fields["Destination Port"] =
//                std::to_string(ntohs(tcpLayer->getTcpHeader()->portDst));
//            break;
//        }
//        case pcpp::UDP: {
//            auto udpLayer = dynamic_cast<pcpp::UdpLayer *>(layer);
//            currLayerInfo.fields["Source Port"] =
//                std::to_string(ntohs(udpLayer->getUdpHeader()->portSrc));
//            currLayerInfo.fields["Destination Port"] =
//                std::to_string(ntohs(udpLayer->getUdpHeader()->portDst));
//            break;
//        }
//        case pcpp::HTTPResponse: {
//            auto httpRspsLayer = dynamic_cast<pcpp::HttpResponseLayer
//            *>(layer); currLayerInfo.fields["HTTP Status Code"] =
//                std::to_string(httpRspsLayer->getFirstLine()->getStatusCode());
//            currLayerInfo.fields["HTTP Status"] =
//                httpRspsLayer->getFirstLine()->getStatusCodeString();
//            break;
//        }
//        case pcpp::HTTPRequest: {
//            auto httpRequestLayer =
//                dynamic_cast<pcpp::HttpRequestLayer *>(layer);
//            currLayerInfo.fields["HTTP Method"] =
//                httpRequestLayer->getFirstLine()->getMethod();
//            currLayerInfo.fields["HTTP URI"] =
//                httpRequestLayer->getFirstLine()->getUri();
//            break;
//        }
//            // ...
//        }
//
//        // Move to the next layer
//        // 如果没有下一层协议了，当前协议就是最顶层的
//        // 存放payload，并退出循环
//        if (!layer->getNextLayer()) {
//            memcpy(currLayerInfo.payload, packet->getRawData(),
//                   packet->getRawDataLen());
//            layersInfo.push_back(currLayerInfo);
//            break;
//        }
//
//        // 继续循环
//        layersInfo.push_back(currLayerInfo);
//        layer = layer->getNextLayer();
//    }
//}

PacketInfo extractPacketInfo(pcpp::RawPacket *rawPacket,
                             const pcpp::Packet &parsedPacket) {
    PacketInfo info;

    // Timestamp
    timespec time = rawPacket->getPacketTimeStamp();
    info.timestamp =
        std::to_string(time.tv_sec) + "." + std::to_string(time.tv_nsec);

    // Packet processing
    pcpp::Packet packet(rawPacket);

    // IP addresses
    if (packet.isPacketOfType(pcpp::IPv4)) {
        pcpp::IPv4Layer *ip4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
        info.srcIp = ip4Layer->getSrcIPv4Address().toString();
        info.dstIp = ip4Layer->getDstIPv4Address().toString();
    } else if (packet.isPacketOfType(pcpp::IPv6)) {
        pcpp::IPv6Layer *ip6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();
        info.srcIp = ip6Layer->getSrcIPv6Address().toString();
        info.dstIp = ip6Layer->getDstIPv6Address().toString();
    }

    // Ports
    if (packet.isPacketOfType(pcpp::TCP)) {
        pcpp::TcpLayer *tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
        info.srcPort = ntohs(tcpLayer->getTcpHeader()->portSrc);
        info.dstPort = ntohs(tcpLayer->getTcpHeader()->portDst);
    } else if (packet.isPacketOfType(pcpp::UDP)) {
        pcpp::UdpLayer *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
        info.srcPort = ntohs(udpLayer->getUdpHeader()->portSrc);
        info.dstPort = ntohs(udpLayer->getUdpHeader()->portDst);
    }

    // Protocol
    info.protocol = getProtocolName(parsedPacket.getLastLayer()->getProtocol());

    // Length
    info.length = rawPacket->getRawDataLen();

    // details
    info.details = "";
    info.details += parsedPacket.toString();
    info.details += toHex(rawPacket->getRawData(), rawPacket->getRawDataLen());

    return info;
}
