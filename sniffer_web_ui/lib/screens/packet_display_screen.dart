import 'dart:async';
import 'dart:convert'; // for json convert
import 'package:flutter/material.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class PacketDisplayScreen extends StatefulWidget {
  final WebSocketChannel channel;
  final StreamController streamController;

  PacketDisplayScreen({required this.channel, required this.streamController});

  @override
  _PacketDisplayScreenState createState() => _PacketDisplayScreenState();
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
  List<Packet> _tempPackets = [];
  final TextEditingController _filterController = TextEditingController();
  String? _selectedPacketDetails;
  StreamSubscription? _streamSubscription;

  @override
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
                  child: Text('开始抓包'),
                ),
              ],
            ),
          ),
          Expanded(
            child: SingleChildScrollView(
              scrollDirection: Axis.horizontal,
              child: DataTable(
                columns: [
                  const DataColumn(label: Text('时间')),
                  const DataColumn(label: Text('IP源地址')),
                  const DataColumn(label: Text('源端口')),
                  const DataColumn(label: Text('目的IP地址')),
                  const DataColumn(label: Text('目的端口')),
                  const DataColumn(label: Text('协议')),
                  const DataColumn(label: Text('长度')),
                ],
                rows: packets
                    .map((packet) => DataRow(
                          cells: [
                            DataCell(Text(packet.time)),
                            DataCell(Text(packet.srcIp)),
                            DataCell(Text(packet.srcPort)),
                            DataCell(Text(packet.dstIp)),
                            DataCell(Text(packet.dstPort)),
                            DataCell(Text(packet.protocol)),
                            DataCell(Text(packet.length)),
                          ],
                          onSelectChanged: (bool? selected) {
                            if (selected != null && selected) {
                              setState(() {
                                _selectedPacketDetails = packet
                                    .details; // assuming `details` is a field in the `packet` object
                              });
                            }
                          },
                        ))
                    .toList(),
              ),
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
      if (_tempPackets.length >= 300) {
          packets.addAll(_tempPackets);
          _tempPackets.clear();
          setState(() {});
      }
    });
  }

  void _doFilter() {}

// 当离开该屏幕或销毁widget时
  @override
  void dispose() {
    widget.channel.sink.add("STOP_SNIFF");
    _streamSubscription?.cancel();
    super.dispose();
  }
}
