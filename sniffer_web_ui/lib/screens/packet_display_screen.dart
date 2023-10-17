import 'package:flutter/material.dart';

class PacketDisplayScreen extends StatefulWidget {
  @override
  _PacketDisplayScreenState createState() => _PacketDisplayScreenState();
}

class _PacketDisplayScreenState extends State<PacketDisplayScreen> {
  // This will hold the packets for demonstration purposes
  List<String> packets = List.generate(100, (index) => 'Packet $index');
  TextEditingController _filterController = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Packets Display")),
      body: Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: TextField(
              controller: _filterController,
              decoration: InputDecoration(
                labelText: 'Filter',
                border: OutlineInputBorder(),
              ),
              onChanged: (value) {
                // Implement your filter logic here
              },
            ),
          ),
          Expanded(
            child: ListView.builder(
              itemCount: packets.length,
              itemBuilder: (context, index) {
                return ListTile(
                  title: Text(packets[index]),
                  // You can add more details about each packet here
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}
