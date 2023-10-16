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
