#include <Husarnet.h>
#include <WiFi.h>
#include <micro_ros_arduino.h>
#include <micro_ros_utilities/string_utilities.h>
#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/string.h>
#include <stdio.h>

#if __has_include("credentials.h")

// For local development (rename credenials-template.h and type your WiFi and
// Husarnet credentials there)
#include "credentials.h"

#else

// For GitHub Actions OTA deploment

// WiFi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// Husarnet credentials
const char *hostName = HUSARNET_HOSTNAME;
const char *husarnetJoinCode = HUSARNET_JOINCODE;  // find at app.husarnet.com
const char *dashboardURL = "default";

#endif

#define RCCHECK(fn)                \
  {                                \
    rcl_ret_t temp_rc = fn;        \
    if ((temp_rc != RCL_RET_OK)) { \
      error_loop();                \
    }                              \
  }

#define RCSOFTCHECK(fn)            \
  {                                \
    rcl_ret_t temp_rc = fn;        \
    if ((temp_rc != RCL_RET_OK)) { \
    }                              \
  }

#define AGENT_PORT 8888
#define NODE_NAME "talker"
char *agent_hostname = "microros-agent";

rcl_publisher_t publisher;
std_msgs__msg__String msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

char buffer[500];

void error_loop() {
  while (1) {
    Serial1.println("error loop");
    delay(100);
  }
}

void setup(void) {
  // ===============================================
  // Wi-Fi and Husarnet VPN configuration
  // ===============================================

  // remap default Serial (used by Husarnet logs)
  Serial.begin(115200, SERIAL_8N1, 16, 17);  // from P3 & P1 to P16 & P17
  Serial1.begin(115200, SERIAL_8N1, 3,
                1);  // remap Serial1 from P9 & P10 to P3 & P1

  Serial1.println("\r\n**************************************");
  Serial1.println("micro-ROS + Husarnet example");
  Serial1.println("**************************************\r\n");

  // Init Wi-Fi
  Serial1.printf("📻 1. Connecting to: %s Wi-Fi network ", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    static int cnt = 0;
    delay(500);
    Serial1.print(".");
    cnt++;
    if (cnt > 10) {
      ESP.restart();
    }
  }

  Serial1.println("done\r\n");

  // Init Husarnet P2P VPN service
  Serial1.printf("⌛ 2. Waiting for Husarnet to be ready ");

  Husarnet.selfHostedSetup(dashboardURL);
  Husarnet.join(husarnetJoinCode, hostName);
  Husarnet.start();

  // Before Husarnet is ready peer list contains:
  // master (0000:0000:0000:0000:0000:0000:0000:0001)
  const uint8_t addr_comp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  bool husarnetReady = 0;
  while (husarnetReady == 0) {
    Serial1.print(".");
    for (auto const &host : Husarnet.listPeers()) {
      if (host.first == addr_comp) {
        ;
      } else {
        husarnetReady = 1;
      }
    }
    delay(1000);
  }

  Serial1.println("done\r\n");

  // ===============================================
  // PLACE YOUR APPLICATION CODE BELOW
  // ===============================================

  Serial1.printf("Known hosts:\r\n");
  for (auto const &host : Husarnet.listPeers()) {
    Serial1.printf("%s (%s)\r\n", host.second.c_str(),
                   host.first.toString().c_str());
  }
  Serial1.println();

  Serial1.printf("⌛ 3. Launching Micro-ROS ");
  set_microros_husarnet_transports(agent_hostname, AGENT_PORT);
  Serial1.printf(".");

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  Serial1.printf(".");

  // create node
  RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));
  Serial1.printf(".");

  // create publisher
  RCCHECK(rclc_publisher_init_best_effort(
      &publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
      "chatter"));
  Serial1.printf(".");

  RCCHECK(rmw_uros_sync_session(5000));

  Serial1.printf("done, sys time = %d\r\n", rmw_uros_epoch_millis());
}


void loop(void) {
  static TickType_t xLastWakeTime = xTaskGetTickCount();
  static int cnt = 0;
  // sprintf(buffer, "Hello World: %d, sys_clk: %d", cnt++, xTaskGetTickCount());
  sprintf(buffer, "Hello World: %d", cnt++);
  Serial1.printf("Publishing: \"%s\" [free heap: %d]\r\n", buffer, xPortGetFreeHeapSize());

  msg.data = micro_ros_string_utilities_set(msg.data, buffer);

  RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

  micro_ros_string_utilities_destroy(&(msg.data));
  vTaskDelayUntil(&xLastWakeTime, 1000);
}