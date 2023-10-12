#include <cstdio>
#include <ctime>
#include <iostream>
#include <pcap.h>
#include <pcap/pcap.h>
#include <string>
#include <thread>
#include <memory>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

/// 处理 packet 的函数
/// usage:
///   通过 pcap_loop(handle,int,`packetHandler`,nullptr) 注册
void packetHandler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ether_header *eth_header;
    struct ip *ip_header;
    char src_host[NI_MAXHOST], dst_host[NI_MAXHOST];

    eth_header = (struct ether_header *)packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        ip_header = (struct ip *)(packet + sizeof(struct ether_header));
        getnameinfo((struct sockaddr *)&ip_header->ip_src, sizeof(ip_header->ip_src), src_host, sizeof(src_host), NULL, 0, NI_NAMEREQD);
        getnameinfo((struct sockaddr *)&ip_header->ip_dst, sizeof(ip_header->ip_dst), dst_host, sizeof(dst_host), NULL, 0, NI_NAMEREQD);

        printf("Source: %s (%s) -> Destination: %s (%s)\n", inet_ntoa(ip_header->ip_src), src_host, inet_ntoa(ip_header->ip_dst), dst_host);
        printf("Bytes: %u\n", pkthdr->len);
        printf("Time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));
    }
}

/// 用于处理 packet 的子线程
/// 防止阻塞主线程
void packetHandleThread(pcap_t *handle) {
  pcap_loop(handle, 100, packetHandler, nullptr);
}

pcap_t* initSniffer(std::string ifname) {
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_t* handle = pcap_open_live(ifname.c_str(), BUFSIZ, 1, 1000, errbuf);
  if (handle == nullptr) {
    std::cerr << "pcap_open_live() failed: " << errbuf << std::endl;
    return nullptr;
  }
  return handle;
}

int main() {
  // 通过传入网卡名称初始化 sniffer ，如果初始化失败以非0值退出
  pcap_t* handle  = initSniffer("eth0");
  if(handle == nullptr){
    perror(" 初始化监听器失败！ ");
    return -1;
  }

  // 开启数据包处理线程
  std::thread captureThread(packetHandleThread, handle);


  // 等待线程结束
  captureThread.join();

  pcap_close(handle);
  printf("done!\n");

  return 0;
}
