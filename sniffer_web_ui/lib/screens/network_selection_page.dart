import 'package:flutter/material.dart';
import 'package:sniffer_web_ui/screens/packet_display_screen.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Wireshark-like Frontend',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: NetworkSelectionPage(),
    );
  }
}

class NetworkSelectionPage extends StatefulWidget {
  @override
  _NetworkSelectionPageState createState() => _NetworkSelectionPageState();
}

class _NetworkSelectionPageState extends State<NetworkSelectionPage> {
  final ipController = TextEditingController();
  final portController = TextEditingController();
  List<String> networkDevices =
      []; // This should be populated after the connection is made.

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Select Network Interface')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            Row(
              children: [
                Expanded(
                    child: TextField(
                        controller: ipController,
                        decoration: InputDecoration(labelText: "IP Address"))),
                SizedBox(width: 10),
                Expanded(
                    child: TextField(
                        controller: portController,
                        decoration: InputDecoration(labelText: "Port"))),
                ElevatedButton(onPressed: _connect, child: Text("Connect"))
              ],
            ),
            SizedBox(height: 20),
            Expanded(
              child: ListView.builder(
                itemCount: networkDevices.length,
                itemBuilder: (context, index) => ListTile(
                  title: Text(networkDevices[index]),
                  // Add more details for each network device if needed
                ),
              ),
            ),
            ElevatedButton(onPressed: _selectDevice, child: Text("Confirm"))
          ],
        ),
      ),
    );
  }

  void _connect() {
    // Implement connection logic here
    // Update the networkDevices list once the connection is made and data is received from the backend.
    // TEST
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => PacketDisplayScreen()),
    );
  }

  void _selectDevice() {
    // Implement logic to process the selected device and proceed to the next screen or step.
  }
}
