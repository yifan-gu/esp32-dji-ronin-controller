// Copyright 2026 Yifan Gu
// SPDX-License-Identifier: Apache-2.0

#include "DJI_ronin_controller.hpp"

#define CAN_TX 12  // Connects to CTX
#define CAN_RX 2  // Connects to CRX
#define CAN_RATE 1000 // Speed rate in kbps

float yaw = 0;
float roll = 0;
float pitch = 0;

DJIRoninController djiRoninController(CAN_TX, CAN_RX, CAN_RATE);

void setup() {
  uint8_t SDK_version[4];

  // Set up serial for debugging
  Serial.begin(115200);

  delay(10000); // XXX(yifan): Wait for the DJI Ronin gimbal to fully start.

  while (!djiRoninController.begin()) {
    DEBUGF("Waiting for DJI Ronin to start...\n");
    delay(1000);
  }

  while (!djiRoninController.get_version(SDK_version)) {
    DEBUGF("Reading DJI Ronin SDK version...\n");
    delay(1000);
  }

  DEBUGF("DJI R SDK Version=%d.%d.%d.%d\n", SDK_version[0], SDK_version[1], SDK_version[2], SDK_version[3]);
}

void loop() {
  if (!djiRoninController.set_position(10, 0, 0, 0, 1000)) {
    Serial.println("Failed to move incrementally");
  }
  delay(1000);

  if (!djiRoninController.get_position(&yaw, &roll, &pitch)) {
    Serial.println("Failed to get position");
  }
  Serial.printf("yaw=%.1f, roll=%.1f, pitch=%.1f\n", yaw, roll, pitch);
  delay(1000);

  if (!djiRoninController.gimbal_active_track()) {
    Serial.println("Failed to gimbal_active_track()");
  } else {
    Serial.println("Success");
  }
  delay(1000);

  if (!djiRoninController.gimbal_recenter()) {
    Serial.println("Failed to recenter()");
  } else {
    Serial.println("Success");
  }
  delay(1000);

  if (!djiRoninController.start_camera_recording()) {
    Serial.println("Failed to start_camera_recording()");
  } else {
    Serial.println("Success start_camera_recording()");
  }
  delay(2000);

  if (!djiRoninController.stop_camera_recording()) {
    Serial.println("Failed to stop_camera_recording()");
  } else {
    Serial.println("Success stop_camera_recording()");
  }
  delay(2000);
}
