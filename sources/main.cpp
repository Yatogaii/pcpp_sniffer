#include <ctime>
#include <iostream>
#include <pcap.h>
#include <pcap/pcap.h>

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr,
                   const u_char *packet) {
  std::cout << "Packet captured at: "
            << std::ctime((const time_t *)&pkthdr->ts.tv_sec);
}

int main() {
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;

  handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
  if (handle == nullptr) {
    std::cerr << "pcap_open_live() failed: " << errbuf << std::endl;
    return 1;
  }

  pcap_close(handle);

  pcap_loop(handle, 10, packetHandler, nullptr);

  pcap_close(handle);

  return 0;
}
