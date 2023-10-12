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

std::unique_ptr<pcap_t> initSniffer(std::string ifname) {
  return nullptr;
}

int main() {
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;

  handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
  if (handle == nullptr) {
    std::cerr << "pcap_open_live() failed: " << errbuf << std::endl;
    return 1;
  }

  std::thread captureThread(packetHandleThread, handle);



  // 在此处，主线程可以执行其他任务
  // ...

  // 等待捕获线程结束
  captureThread.join();

  pcap_close(handle);

  printf("done!\n");

  return 0;
}
