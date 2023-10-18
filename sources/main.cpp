#include <PcapLiveDevice.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// #include "App.h"
#include "uWebSockets/App.h"

#include "packets.h"
#include "protocols.h"
#include "sockets.h"

SyncQueue<std::string> syncQueue;
bool isCapture = false;

/// formatLayerInfos, 基本上一个 packet 就对应一个LayerInfo ，
/// payload什么的也都存放在这里
/// 该函数主要处理数据报格式到方便 socket 发送的格式。
/// TODO 格式还需要确定

std::string formatLayerInfos(const std::vector<LayerInfo> &layerInfos) {
    nlohmann::json jsonArray;

    for (const auto &info : layerInfos) {
        nlohmann::json jsonObject;
        jsonObject["protocolName"] = info.protocolName;
        for (const auto &[key, value] : info.fields) {
            jsonObject["fields"][key] = value;
        }
        jsonObject["details"] = "details";
        jsonArray.push_back(jsonObject);
    }

    return jsonArray.dump(); // 把jsonArray 转化为 string
}

/// 数据包处理示例函数
void printPacketInfo(const pcpp::Packet &packet) {
    // Step1: 获取 Packet 的协议栈
    std::vector<pcpp::ProtocolType> protocols = getProtocolStack(packet);

    pcpp::IPv4Layer *ipv4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::IPv6Layer *ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();
    pcpp::TcpLayer *tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    pcpp::UdpLayer *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
    pcpp::DnsLayer *dnsLayer = packet.getLayerOfType<pcpp::DnsLayer>();
    pcpp::HttpRequestLayer *httReqLayer =
        packet.getLayerOfType<pcpp::HttpRequestLayer>();
    pcpp::HttpResponseLayer *httRspLayer =
        packet.getLayerOfType<pcpp::HttpResponseLayer>();

    // Print basic IP/TCP/UDP information
    if (ipv4Layer != nullptr) {
        std::cout << "IP packet: " << ipv4Layer->getSrcIPAddress() << " -> "
                  << ipv4Layer->getDstIPAddress();
        if (tcpLayer != nullptr) {
            std::cout << " UDP " << udpLayer->getSrcPort() << " -> "
                      << udpLayer->getDstPort();
        }
        std::cout << std::endl;
    }

    // Print DNS information
    if (dnsLayer != nullptr) {
        std::cout << "DNS query: " << dnsLayer->getQueryCount() << " queries"
                  << std::endl;
    }
}

/// 数据包处理函数
void onPacketArrives(pcpp::RawPacket *packet, pcpp::PcapLiveDevice *dev,
                     void *cookie) {
    // Step0: 把 Raw 解析成 Packet
    pcpp::Packet parsedPacket(packet);

    // TODO 是否需要同步队列进行多线程处理？

    // Step1: 获取 Packet 的协议栈
    std::vector<pcpp::ProtocolType> protocols = getProtocolStack(packet);

    // Step2: 处理所有协议，每个协议都读取相关的信息
    // TODO 尚未实现，这一步到底需要存放哪些信息不好确定
    // printPacketInfo(parsedPacket);
    std::vector<LayerInfo> packetLayerInfos;
    parseLayer(parsedPacket.getFirstLayer(), packet, packetLayerInfos);

    // Step3: 把处理到的数据包发给前端进行解析
    // 需要确定数据包拼接的形式
    std::string serialLayerInfos = formatLayerInfos(packetLayerInfos);
    std::cout << serialLayerInfos;
    syncQueue.push(serialLayerInfos);
}

/// 获得当前设备的所有网卡，返回给主程序
/// 获得的网卡经过用户选择后再确定具体抓包流量来自哪一个网卡
std::vector<pcpp::PcapLiveDevice *> getLiveDevices() {
    return pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
}

int main() {
    // Step0: 初始化Socket，与前端通过socket通信
    // 基本流程：
    //   监听-收到连接-发送网卡列表-选择网卡-开始抓包
    //   必须换成WebSocket了
    /// Socket serverSocket;
    /// if (!serverSocket.Create()) {
    ///     std::cerr << "Error creating server socket." << std::endl;
    ///     return 1;
    /// }
    /// if (!serverSocket.Bind("127.0.0.1", 28080)) {
    ///     std::cerr << "Error binding to address." << std::endl;
    ///     return 1;
    /// }
    /// if (!serverSocket.Listen()) {
    ///     std::cerr << "Error listening for connections." << std::endl;
    ///     return 1;
    /// }
    /// Socket clientSocket;
    /// if (!serverSocket.Accept(clientSocket)) {
    ///     // TODO
    /// }

    // Step0: 获取网卡设备列表
    std::vector<pcpp::PcapLiveDevice *> devLists = getLiveDevices();
    /// TEST: Done
    /// for (auto each : devLists) {
    ///    std::cout << each->getName() << std::endl
    ///}
    /// return 0;

    // Step1: 选择设备
    // Step1.1: 拼接列表字符串
    std::string devDatas;
    for (auto each : devLists) {
        devDatas += each->getName();
        devDatas += ",";
        devDatas += each->getIPv4Address().toString();
        devDatas += ";";
    }

    // std::vector<std::string> devDatas;
    // for (auto each : devLists) {
    //     devDatas.push_back(each->getName());
    //     devDatas.push_back(",");
    // }
    // Step1.2: 发送给前端供用户选择
    // clientSocket.Send(DataPack(devDatas, DataPack::kDataTypeDevList));

    // Step1.3: 接受前端返回的数据
    std::string recvData;
    // clientSocket.Receive(recvData);
    // 判断数据是否合理
    // TODO 可能需要对recvDtas 进行提取
    // if (std::find(devDatas.begin(), devDatas.end(), recvData) ==
    //     devDatas.end()) {
    //     // TODO 接受到的数据不在网卡列表里，错误处理
    // }

    // Step2:  建立websocket
    // uWS::App()
    //    .ws<char>("/*",
    //              {/* .open handler */
    //               .open =
    //                   [](auto *ws) {
    //                       std::cout << "Client connected!" << std::endl;
    //                   },

    //               /* .message handler */
    //               .message =
    //                   [devDatas](auto *ws, std::string_view message,
    //                              uWS::OpCode opCode) {
    //                       if (message == "GET_DEVICE") {
    //                           // This is just an example of a loop that
    //                           // sends a message every second. You can
    //                           // adjust it according to your
    //                           // requirements.
    //                           ws->send(devDatas.c_str(), opCode);
    //                       } else if (message == "START_SNIFF") {
    //                           // Handle other types of messages here
    //                           std::cout << "start sniff recived";
    //                           isCapture = true;
    //                           while (true) {
    //                               ws->send(syncQueue.pop().c_str(), opCode);
    //                           }
    //                           // 这里开始通过同步队列不断的向客户端发送消息
    //                       } else if (message == "STOP_SNIFF") {
    //                           isCapture = false;
    //                       }
    //                   },

    //               /* .close handler */
    //               .close =
    //                   [](auto *ws, int code, std::string_view message) {
    //                       std::cout << "Client disconnected!" << std::endl;
    //                   }})
    //    .listen(28080,
    //            [](auto *listen_socket) {
    //                if (listen_socket) {
    //                    std::cout << "Server is listening on port 28080"
    //                              << std::endl;
    //                } else {
    //                    std::cerr << "Error listening on port 28080"
    //                              << std::endl;
    //                    exit(1);
    //                }
    //            })
    //    .run();

    // Step3: 开个子线程开始抓包，主线程接受进一步的消息
    // pcpp::PcapLiveDevice* dev =
    // pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(recvData.c_str());
    // pcpp::PcapLiveDevice *dev =
    //    pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(
    //        "172.24.27.33");
    // if (!dev) {
    //    std::cerr << "Cannot find specific device!" << std::endl;
    //    return 1;
    //}

    // if (!dev->open()) {
    //     std::cerr << "Cannot open device!" << std::endl;
    //     return 1;
    // }

    // printf("start cap");
    // dev->startCapture(onPacketArrives, nullptr);

    pcpp::PcapLiveDevice *dev;
    uWS::App()
        .ws<char>(
            "/*",
            {/* .open handler */
             .open =
                 [](auto *ws) {
                     std::cout << "Client connected!" << std::endl;
                 },

             /* .message handler */
             .message =
                 [devDatas, dev](auto *ws, std::string_view message,
                                 uWS::OpCode opCode) mutable {
                     if (message == "GET_DEVICES") {
                         // This is just an example of a loop that
                         // sends a message every second. You can
                         // adjust it according to your
                         // requirements.
                         std::cout << "get device";
                         ws->send(devDatas.c_str(), opCode);
                     } else if (message == "START_SNIFF") {
                         // Handle other types of messages here
                         std::cout << "start sniff recived";
                         // dev = pcpp::PcapLiveDeviceList::getInstance()
                         // .getPcapLiveDeviceByIp("172.24.27.33");
                         printf("start cap");
                         // TODO 这里需要加错误处理
                         dev->startCapture(onPacketArrives, nullptr);
                         isCapture = true;
                         while (true) {
                             ws->send(syncQueue.pop().c_str(), opCode);
                         }
                         // 这里开始通过同步队列不断的向客户端发送消息
                     } else if (message == "STOP_SNIFF") {
                         isCapture = false;
                         // 认为是接受到的网卡名称
                     } else {
                         std::string name = {message.begin(), message.end()};
                         dev = pcpp::PcapLiveDeviceList::getInstance()
                                   .getPcapLiveDeviceByName(name);
                         if (!dev) {
                             std::cerr << "Cannot find specific device!"
                                       << std::endl;
                             return;
                         }

                         if (!dev->open()) {
                             std::cerr << "Cannot open device!" << std::endl;
                             return;
                         }
                     }
                 },

             /* .close handler */
             .close =
                 [](auto *ws, int code, std::string_view message) {
                     std::cout << "Client disconnected!" << std::endl;
                 }})
        .listen(28080,
                [](auto *listen_socket) {
                    if (listen_socket) {
                        std::cout << "Server is listening on port 28080"
                                  << std::endl;
                    } else {
                        std::cerr << "Error listening on port 28080"
                                  << std::endl;
                        exit(1);
                    }
                })
        .run();
    // Step4: 主线程不再需要监听消息，通过uWS来进行通信
    while (isCapture)
        ;

    // Step5: 退出程序，或者重置状态后等待下一次抓包
    // TODO

    // dev->stopCapture();
    // dev->close();

    return 0;
}
