# esp32-microros-husarnet

ESP32 + Micro-ROS + Husarnet demo

> **Prerequisites** 
>
> Install [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode).

## Quick start

### ESP32 (ROS 2 talker)

1. Clone the repo  and open it in Visual Studio Code. Platformio should automatically install all project dependencies.

2. Rename `credentials-template.h` to `credentials.h` and type your WiFi an Husarnet credentials there (you will find you Husarnet Join Code at https://app.husarnet.com).

4. Click "PlatformIO: upload" button to flash your ESP32 board connected to your laptop. You will find the following log in the serial monitor:

    ```bash
    **************************************
    micro-ROS + Husarnet example
    **************************************

    📻 1. Connecting to: UPC5C2DB59 Wi-Fi network . done

    ⌛ 2. Waiting for Husarnet to be ready ........ done

    Known hosts:
    esp32-talker (fc94:4050:1fc5:dc02:d27f:97b9:7f13:e215)
    master (fc94:4050:1fc5:dc02:d27f:97b9:7f13:e215)
    microros-agent (fc94:ffd0:3cd7:d104:2917:ae5c:a719:9e71)
    Connecting to "microros-agent:8888"... 
    ```

### Laptop (ROS 2 listener + Micro-ROS Agent)

1. Rename `.env.template` to `.env` and place Husarnet Join Code here (the same as provided for ESP32 before).

2. Launch `micro-ROS agent` (TCPv6 on port **8888**), `listener` (from **demo_nodes_cpp**) and Husarnet container for VPN connectivity:

```bash
cd demo
docker-compose up
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
