#include <ctime>
#include <iostream>
#include <pcap.h>
#include <pcap/pcap.h>
#include <string>
#include <thread>
#include <memory>

/// 处理 packet 的函数
/// usage:
///   通过 pcap_loop(handle,int,`packetHandler`,nullptr) 注册
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr,
                   const u_char *packet) {
  std::cout << "Packet captured at: "
            << std::ctime((const time_t *)&pkthdr->ts.tv_sec);
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

  // 开启数据包处理线程
  std::thread captureThread(packetHandleThread, handle);


  // 等待线程结束
  captureThread.join();

  pcap_close(handle);
  printf("done!\n");

  return 0;
}
