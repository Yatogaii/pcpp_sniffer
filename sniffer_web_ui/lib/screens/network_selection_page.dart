import 'package:flutter/material.dart';
import 'package:sniffer_web_ui/screens/packet_display_screen.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

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

// 假设NetworkDevice是设备的数据模型
class NetworkDevice {
  final String name;
  final String ip;

  NetworkDevice(this.name, this.ip);
}

class NetworkSelectionPage extends StatefulWidget {
  @override
  _NetworkSelectionPageState createState() => _NetworkSelectionPageState();
}

class _NetworkSelectionPageState extends State<NetworkSelectionPage> {
  late TextEditingController _ipController;
  late TextEditingController _portController;
  late WebSocketChannel _channel;

  @override
  void initState() {
    super.initState();
    _ipController = TextEditingController();
    _portController = TextEditingController();
  }

  List<NetworkDevice> networkDevices = []; // 用于存储设备列表的变量
  NetworkDevice? selectedDevice; // 用于跟踪所选设备

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
            DataTable(
              columns: [
                DataColumn(label: Text('网卡名称')),
                DataColumn(label: Text('IP地址')),
              ],
              rows: networkDevices
                  .map((device) => DataRow(
                        selected: selectedDevice == device,
                        onSelectChanged: (bool? value) {
                          setState(() {
                            if (value == true) {
                              selectedDevice = device;
                            } else {
                              selectedDevice = null;
                            }
                          });
                        },
                        cells: [
                          DataCell(Text(device.name)),
                          DataCell(Text(device.ip)),
                        ],
                      ))
                  .toList(),
//              networkDevices
//                  .map((device) => DataRow(cells: [
//                        DataCell(Text(device.name)),
//                        DataCell(Text(device.ip)),
//                      ]))
//                  .toList(),
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
      _channel = WebSocketChannel.connect(Uri.parse('ws://$host:$port'));

// 发送 "GET_DEVICE" 消息给服务器
      _channel.sink.add("GET_DEVICES");

// 监听来自服务器的消息
      _channel.stream.listen((data) {
        print(data);
        final serverResponse = data.trim();
        final deviceEntries = serverResponse.split(';');

        List<NetworkDevice> devices = [];
        for (var entry in deviceEntries) {
          final parts = entry.split(',');
          if (parts.length >= 2) {
            // 确保数据格式是正确的
            devices.add(NetworkDevice(parts[0], parts[1]));
          }
        }
        // 更新界面
        setState(() {
          networkDevices = devices;
        });
      });
    } catch (error) {
      print('Unable to connect: $error');
      // Handle the error, maybe show a message to the user
    }
  }

  // handle confirm button
  void _selectDevice() {
    if (selectedDevice != null) {
      print(
          "Confirmed selection: ${selectedDevice!.name}, ${selectedDevice!.ip}");
      // 这里处理所选设备
    } else {
      print("No device selected.");
      return;
    }
    // Implement logic to process the selected device and proceed to the next screen or step.
    Navigator.push(
      context,
      MaterialPageRoute(
          builder: (context) => PacketDisplayScreen(channel: _channel)),
    );
  }
}
