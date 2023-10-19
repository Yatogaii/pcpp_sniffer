import 'package:flutter/material.dart';
import 'package:url_launcher/url_launcher.dart';
import 'package:sniffer_web_ui/screens/network_selection_page.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: '抓包器',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatelessWidget {
  // 这个函数用于启动URL
  _launchURL() async {
    const url = 'https://github.com/Yatogaii/pcpp_sniffer/tree/main';
    if (await canLaunchUrl(Uri.parse(url))) {
      await launchUrl(Uri.parse(url));
    } else {
      throw '无法启动 $url';
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('抓包器'),
      ),
      body: Column(
        children: [
          Expanded(
            flex: 3,
            child: Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  const Text(
                    '抓包器',
                    style: TextStyle(
                      fontSize: 24.0,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  const SizedBox(height: 20.0),
                  ElevatedButton(
                    onPressed: () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                            builder: (context) => NetworkSelectionPage()),
                      );
                    },
                    child: const Text('开始抓包'),
                  ),
                ],
              ),
            ),
          ),
          Expanded(
            flex: 1,
            child: Column(
              mainAxisAlignment: MainAxisAlignment.end,
              children: <Widget>[
                InkWell(
                  onTap: _launchURL,
                  child: Text(
                    'Github: Yatogaii/pcpp_sniffer',
                    style: const TextStyle(
                      color: Colors.blue,
                      decoration: TextDecoration.underline,
                    ),
                  ),
                ),
                const Text('Ver 1.0.0'),
                const SizedBox(height: 10.0), // 为了确保相关信息与底部有一些间隔
              ],
            ),
          ),
        ],
      ),
    );
  }
}
