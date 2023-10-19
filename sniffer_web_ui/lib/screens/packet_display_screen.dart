import 'dart:async';
import 'dart:convert'; // for json convert
import 'package:flutter/material.dart';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'dart:math';

class PacketDisplayScreen extends StatefulWidget {
  final WebSocketChannel channel;
  final StreamController streamController;

  PacketDisplayScreen({required this.channel, required this.streamController});

  @override
  _PacketDisplayScreenState createState() => _PacketDisplayScreenState();
}

class PacketListItem extends StatelessWidget {
  final Packet packet;
  final Function(Packet) onSelect;

  PacketListItem({required this.packet, required this.onSelect});

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: () => onSelect(packet),
      child: Container(
        decoration: BoxDecoration(
          border: Border(
            bottom: BorderSide(color: Colors.grey),
            right: BorderSide(color: Colors.grey),
          ),
        ),
        child: Row(
          children: [
            Expanded(child: Text(packet.time)),
            Expanded(child: Text(packet.srcIp)),
            Expanded(child: Text(packet.srcPort)),
            Expanded(child: Text(packet.dstIp)),
            Expanded(child: Text(packet.dstPort)),
            Expanded(child: Text(packet.protocol)),
            Expanded(child: Text(packet.length)),
          ],
        ),
      ),
    );
  }
}

class Packet {
  final String time;
  final String srcIp;
  final String srcPort;
  final String dstIp;
  final String dstPort;
  final String protocol;
  final String length;
  final String details;

  Packet(this.time, this.srcIp, this.srcPort, this.dstIp, this.dstPort,
      this.protocol, this.length, this.details);
}

class _PacketDisplayScreenState extends State<PacketDisplayScreen> {
  List<Packet> packets = [];
  List<Packet> _bakcupPackets = [];
  List<Packet> _tempPackets = [];
  List<Packet> _filterdPackets = [];

  final TextEditingController _filterController = TextEditingController();
  String? _selectedPacketDetails;
  StreamSubscription? _streamSubscription;
  // 判断是否正在过滤，辅助处理新来的数据报
  bool isFiltering = false;
  // 判断是否已经开始 sniff ，辅助按钮操作。
  bool isSniffing = false;

  String buttonSnifferText = '开始抓包';

  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Packets Display")),
      body: Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: Row(
              children: <Widget>[
                Expanded(
                  child: TextField(
                    controller: _filterController,
                    decoration: const InputDecoration(
                      labelText: 'Filter',
                      border: OutlineInputBorder(),
                    ),
                    onChanged: (value) {
                      // Implement your filter logic here if needed
                    },
                  ),
                ),
                const SizedBox(width: 10),
                ElevatedButton(
                  onPressed: _doFilter,
                  child: const Text('筛选'),
                ),
                const SizedBox(width: 10),
                ElevatedButton(
                  onPressed: _startSniffer,
                  child: Text(buttonSnifferText),
                ),
              ],
            ),
          ),
          // 表头
          Container(
            decoration: BoxDecoration(
              border: Border.all(color: Colors.grey),
              color: Colors.grey[300], // 背景色
            ),
            child: Padding(
              padding: EdgeInsets.all(8.0),
              child: Row(
                children: [
                  Expanded(child: Text('时间')),
                  Expanded(child: Text('IP源地址')),
                  Expanded(child: Text('源端口')),
                  Expanded(child: Text('目的IP地址')),
                  Expanded(child: Text('目的端口')),
                  Expanded(child: Text('协议')),
                  Expanded(child: Text('长度')),
                ],
              ),
            ),
          ),
          // 数据
          Expanded(
            child: ListView.builder(
              itemCount: packets.length,
              itemBuilder: (context, index) {
                final packet = packets[index];
                return PacketListItem(
                  packet: packet,
                  onSelect: (selectedPacket) {
                    setState(() {
                      _selectedPacketDetails = selectedPacket.details;
                    });
                  },
                );
              },
            ),
          ),
          Divider(),
          Expanded(
            child: SingleChildScrollView(
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: Text(_selectedPacketDetails ??
                    "Select a packet to view details."),
              ),
            ),
          ),
        ],
      ),
    );
  }

  void _startSniffer() {
    // 如果没有抓包
    if (!isSniffing) {
      isSniffing = true;
      buttonSnifferText = '停止抓包';
      print("start sniff");
      widget.channel.sink.add("START_SNIFF");
      // widget.channel.sink.add("GET_DEVICES");

      // 注册消息处理函数
      _streamSubscription = widget.streamController.stream.listen((data) {
        Map<String, dynamic> packetObj = jsonDecode(data);
        // 判断是否转化成功
        //if (packetObj["success"]) {
        _tempPackets.add(Packet(
            packetObj["time"],
            packetObj["srcIp"],
            packetObj["srcPort"],
            packetObj["dstIp"],
            packetObj["dstPort"],
            packetObj["protocol"],
            packetObj["length"],
            packetObj["details"]));
        if (_tempPackets.length >= 0) {
          // 如果正在筛选，不加入packets, 不 setState
          if (isFiltering) {
            _bakcupPackets.addAll(_tempPackets);
            _tempPackets.clear();
          } else {
            packets.addAll(_tempPackets);
            _tempPackets.clear();
            setState(() {});
          }
        }
      });
      // 如果正在抓包
    } else {
      isSniffing = false;
      buttonSnifferText = '开始抓包';
      _streamSubscription?.cancel();
      setState(() {
      });
    }
  }

  // For  filter function
  List<Packet> filterPacketsChunk(List<Packet> chunk, String filter) {
    var conditions = filter.split('and');
    List<Packet> results = List.from(chunk); // 初始列表为所有数据包

    for (var condition in conditions) {
      condition = condition.trim();

      if (condition.startsWith('ip=')) {
        var ip = condition.split('=')[1].trim();
        results = results
            .where((packet) => packet.srcIp == ip || packet.dstIp == ip)
            .toList();
      } else if (condition.startsWith('port=')) {
        var port = condition.split('=')[1].trim();
        results = results
            .where((packet) => packet.srcPort == port || packet.dstPort == port)
            .toList();
      } else if (condition.startsWith('protocol=')) {
        var proto = condition.split('=')[1].trim();
        results = results.where((packet) => packet.protocol == proto).toList();
      }
    }

    return results;
  }

  Future<List<Packet>> parallelFilter(String filter) {
    const chunkSize = 3000; // Adjust based on your requirements
    var numChunks = (packets.length / chunkSize).ceil();

    List<Future<List<Packet>>> futures = [];
    for (var i = 0; i < numChunks; i++) {
      var start = i * chunkSize;
      // var end = start + chunkSize;
      var end = min(start + chunkSize,
          packets.length); // Ensure end doesn't exceed the list size
      futures.add(Future(
          () => filterPacketsChunk(packets.sublist(start, end), filter)));
    }

    return Future.wait(futures).then((List<List<Packet>> results) {
      return results.expand((packets) => packets).toList();
    });
  }

  void _doFilter() async {
    final String filter = _filterController.text;
    // 恢复原状
    if (filter.trim().isEmpty) {
      isFiltering = false;
      print("empty filter");
      packets.addAll(_bakcupPackets);
      _bakcupPackets.clear();
      setState(() {});
    } else {
      // 开始执行筛选
      isFiltering = true;
      _bakcupPackets.addAll(packets);
      packets = await parallelFilter(filter);
      setState(() {});
    }
  }

// 当离开该屏幕或销毁widget时
  @override
  void dispose() {
    widget.channel.sink.add("STOP_SNIFF");
    _streamSubscription?.cancel();
    super.dispose();
  }
}
