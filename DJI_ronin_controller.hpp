// Copyright 2026 Yifan Gu
// SPDX-License-Identifier: Apache-2.0

#ifndef DJI_RONIN_CONTROLLER_HPP
#define DJI_RONIN_CONTROLLER_HPP

#include <ESP32-TWAI-CAN.hpp>
#include <Arduino.h>

#define DEBUGF(...) //Serial.printf(__VA_ARGS__)
#define WARNF(...) Serial.printf(__VA_ARGS__)

#define SOF_BYTE 0xAA
#define FRAME_LEN 8
#define NORMAL_SEND 0
#define DATA_FRAME 1
#define STD_FRAME 0

#define MASK_BIT_TYPE 0xFF
#define MAX_INT_8 256

#define DATA_BUFFER_SIZE 8 * FRAME_LEN

class DJIRoninController {
  private:
    TwaiCAN esp32Can;
    int8_t tx_pin;
    int8_t rx_pin;
    int16_t rate_speed;
    uint16_t seq_num = 0;
    uint32_t can_send_id = 0x223;
    uint32_t can_recv_id = 0x222;

    bool _send_data(uint8_t *data, size_t data_len);
    size_t _assemble_can_msg(uint8_t cmd_type, uint8_t cmd_set, uint8_t cmd_id, const uint8_t* data, size_t data_len, uint8_t *can_msg);
    uint16_t _seq_num();
    bool _recv_data(uint8_t *data);
    bool _execute_command(uint8_t cmd_type, uint8_t cmd_set, uint8_t cmd_id, uint8_t *data, size_t data_size, uint8_t *recv_data);
  public:
    DJIRoninController(int txPin, int rxPin, int rateSpeed) { tx_pin = txPin; rx_pin = rxPin; rate_speed=rateSpeed; }
    bool begin();
    bool set_position(float yaw, float roll, float pitch, bool absolute_position, uint16_t time_for_action_in_millis);
    bool get_position(float *yaw, float *roll, float *pitch);
    bool get_version(uint8_t version[4]);
    bool gimbal_active_track();
    bool gimbal_recenter();
    bool start_camera_recording();
    bool stop_camera_recording();

    void dump_frame();
};

#endif
