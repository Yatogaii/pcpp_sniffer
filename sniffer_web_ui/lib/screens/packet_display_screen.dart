import 'package:flutter/material.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class PacketDisplayScreen extends StatefulWidget {
  final WebSocketChannel channel;

  PacketDisplayScreen({required this.channel});
  @override
  _PacketDisplayScreenState createState() => _PacketDisplayScreenState();
}

class Packet {
  final String time;
  final String srcIP;
  final String srcPort;
  final String dstIP;
  final String dstPort;
  final String protocol;
  final String length;
  final String details;

  Packet(this.time, this.srcIP, this.srcPort, this.dstIP, this.dstPort,
      this.protocol, this.length, this.details);
}

class _PacketDisplayScreenState extends State<PacketDisplayScreen> {

  List<Packet> packets = [];
  TextEditingController _filterController = TextEditingController();
  String?  _selectedPacketDetails;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Packets Display")),
      body: Column(
        children: <Widget>[
          Padding(
          padding: const EdgeInsets.all(8.0),
          child: Row(
            children: <Widget>[
              Expanded(
                child: TextField(
                  controller: _filterController,
                  decoration: InputDecoration(
                    labelText: 'Filter',
                    border: OutlineInputBorder(),
                  ),
                  onChanged: (value) {
                    // Implement your filter logic here if needed
                  },
                ),
              ),
              SizedBox(width: 10),
              ElevatedButton(
                onPressed: () {
                  // Implement your filtering logic here
                },
                child: Text('筛选'),
              ),
              SizedBox(width: 10),
              ElevatedButton(
                onPressed: () {
                  // Implement your start capturing logic here
                },
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
                            DataCell(Text(packet.srcIP)),
                            DataCell(Text(packet.srcPort)),
                            DataCell(Text(packet.dstIP)),
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

  //@override
  //Widget build(BuildContext context) {
  //  return Scaffold(
  //    appBar: AppBar(title: Text("Packets Display")),
  //    body: Column(
  //      children: <Widget>[
  //        Padding(
  //          padding: const EdgeInsets.all(8.0),
  //          child: TextField(
  //            controller: _filterController,
  //            decoration: InputDecoration(
  //              labelText: 'Filter',
  //              border: OutlineInputBorder(),
  //            ),
  //            onChanged: (value) {
  //              // Implement your filter logic here
  //            },
  //          ),
  //        ),
  //        Expanded(
  //          child: ListView.builder(
  //            itemCount: packets.length,
  //            itemBuilder: (context, index) {
  //              return ListTile(
  //                title: Text(packets[index]),
  //                // You can add more details about each packet here
  //              );
  //            },
  //          ),
  //        ),
  //      ],
  //    ),
  //  );
  //}
}
