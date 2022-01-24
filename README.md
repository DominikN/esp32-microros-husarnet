# esp32-microros-husarnet

[![Build a firmware](https://github.com/DominikN/esp32-microros-husarnet/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/DominikN/esp32-microros-husarnet/actions/workflows/build.yml)

ESP32 + Micro-ROS + Husarnet demo

> **Prerequisites** 
>
> Install [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode).

## Quick start

Clone the repo  and open it in Visual Studio Code. Platformio should automatically install all project dependencies

### Laptop (ROS 2 listener + Micro-ROS Agent)

1. In a `demo/` folder rename `.env.template` to `.env` and place Husarnet Join Code here (you will findyour **Husarnet Join Code at https://app.husarnet.com)**.

2. Launch `micro-ROS agent` (TCPv6 on port **8888**), `listener` (from **demo_nodes_cpp**) and Husarnet container for VPN connectivity:

```bash
cd demo
docker-compose up
```

### ESP32 (ROS 2 talker)

1. Rename `credentials-template.h` to `credentials.h` and type your WiFi an Husarnet Join Code here (the same as in `.env` file before) 

2. Click "PlatformIO: upload" button to flash your ESP32 board connected to your laptop. You will find the following log in the serial monitor:

    ```bash
    **************************************
    micro-ROS + Husarnet example
    **************************************

    📻 1. Connecting to: FreeWifi Wi-Fi network . done

    ⌛ 2. Waiting for Husarnet to be ready ........ done

    Known hosts:
    esp32-talker (fc94:4050:1fc5:dc02:d27f:97b9:7f13:e215)
    master (fc94:4050:1fc5:dc02:d27f:97b9:7f13:e215)
    microros-agent (fc94:ffd0:3cd7:d104:2917:ae5c:a719:9e71)
    ```

## Tips

### Erasing flash memory of ESP32

1. Connect ESP32 to your laptop

2. Install platformio CLI

    ```bash
    pip install -U platformio
    ```

3. Make flash erase:

    ```bash
    pio run --target erase
    ```

### Monitoring network traffic on `hnet0` interface

```bash
sudo tcpflow -p -c -i hnet0
```
