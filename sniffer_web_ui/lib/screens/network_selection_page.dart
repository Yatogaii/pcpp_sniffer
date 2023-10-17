import 'package:flutter/material.dart';
import 'package:sniffer_web_ui/screens/packet_display_screen.dart';
import 'dart:io';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Network Interface Selection Page',
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
  late TextEditingController _ipController;
  late TextEditingController _portController;
  Socket? _socket;

  @override
  void initState() {
    super.initState();
    _ipController = TextEditingController();
    _portController = TextEditingController();
  }

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
                        controller: _ipController,
                        decoration: InputDecoration(labelText: "IP Address"))),
                SizedBox(width: 10),
                Expanded(
                    child: TextField(
                        controller: _portController,
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

  void _connect() async {
    final host = _ipController.text;
    final port = int.tryParse(_portController.text);

    if (port == null) {
      // Handle error: Invalid port number
      print("Invalid port number");
      return;
    }

    try {
      _socket = await Socket.connect(host, port);

      // Once connected, listen to the data from the server
      _socket!.listen((List<int> data) {
        final serverResponse = String.fromCharCodes(data).trim();
        final devices = serverResponse.split(',');
        print(serverResponse);

        // You can now use 'devices' list to display or for further processing
      }, onError: (error) {
        print("Error: $error");
        // Handle the error, maybe show a message to the user
      }, onDone: () {
        _socket!.close();
      });
    } catch (error) {
      print('Unable to connect: $error');
      // Handle the error, maybe show a message to the user
    }
  }

  void _selectDevice() {
    print("test");
    // Implement logic to process the selected device and proceed to the next screen or step.
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => PacketDisplayScreen()),
    );
  }
}
