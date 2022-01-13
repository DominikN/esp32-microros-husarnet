#if defined(ESP32) || defined(TARGET_PORTENTA_H7_M7) || \
    defined(ARDUINO_NANO_RP2040_CONNECT)



#if defined(ESP32) || defined(TARGET_PORTENTA_H7_M7)
#include <Arduino.h>
#include <AsyncTCP.h>
#elif defined(ARDUINO_NANO_RP2040_CONNECT)
#include <SPI.h>
#include <WiFiNINA.h>
#endif

#include <micro_ros_arduino.h>

QueueHandle_t rx_queue;

extern "C" {

static AsyncClient *client_tcp = new AsyncClient;

bool arduino_wifi_transport_open(struct uxrCustomTransport *transport) {
  struct micro_ros_agent_locator *locator =
      (struct micro_ros_agent_locator *)transport->args;

  rx_queue = xQueueCreate(1000, sizeof(uint8_t));

  client_tcp->onConnect(
      [](void *arg, AsyncClient *client) {
        Serial.println("[CALLBACK] connected");
      },
      client_tcp);

  client_tcp->onData(
      [](void *arg, AsyncClient *client, void *data, size_t len) {
        Serial.printf("\r\nResponse from %s\r\n",
                      client->remoteIP().toString().c_str());
        Serial.write((uint8_t *)data, len);
        for (int i = 0; i < len; i++) {
          xQueueSendFromISR(rx_queue, data + i, NULL);
        }
        client->close();
      },
      client_tcp);

  client_tcp->onDisconnect(
      [](void *arg, AsyncClient *client) {
        Serial.println("[CALLBACK] discconnected");
        delete client;
      },
      client_tcp);

  client_tcp->onError(
      [](void *arg, AsyncClient *client, int8_t error) {
        Serial.printf("[CALLBACK] error: %d\r\n", error);
      },
      NULL);

  client_tcp->onTimeout(
      [](void *arg, AsyncClient *client, uint32_t time) {
        Serial.println("[CALLBACK] ACK timeout");
      },
      NULL);

  client_tcp->connect(locator->hostname, locator->port);

  return true;
}

bool arduino_wifi_transport_close(struct uxrCustomTransport *transport) {
  client_tcp->close();
  return true;
}

size_t arduino_wifi_transport_write(struct uxrCustomTransport *transport,
                                    const uint8_t *buf, size_t len,
                                    uint8_t *errcode) {
  (void)errcode;
  struct micro_ros_agent_locator *locator =
      (struct micro_ros_agent_locator *)transport->args;

  if (client_tcp->canSend() && (client_tcp->space() > len)) {
    client_tcp->add((const char *)buf, len);
    client_tcp->send();
  } else {
    Serial.printf("\r\nSENDING ERROR!\r\n");
  }

  return len;
}

size_t arduino_wifi_transport_read(struct uxrCustomTransport *transport,
                                   uint8_t *buf, size_t len, int timeout,
                                   uint8_t *errcode) {
  (void)errcode;

  int retval = 0;

  for (int i = 0; i < len; i++) {
    if (pdTRUE == xQueueReceive(rx_queue, buf + i, timeout)) {
      retval++;
    }
  }

  return retval;
}
}

#endif
