// Copyright 2026 Yifan Gu
// SPDX-License-Identifier: Apache-2.0

#include "DJI_ronin_controller.hpp"

// CRC16 Lookup Table
unsigned short CRC16_Table[256] = {
  0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
  0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
  0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
  0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
  0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
  0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
  0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
  0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
  0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
  0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
  0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
  0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
  0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
  0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
  0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
  0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
  0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
  0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
  0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
  0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
  0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
  0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
  0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
  0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
  0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
  0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
  0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
  0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
  0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
  0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
  0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
  0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

// CRC32 Lookup Table
unsigned int CRC32_Table[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};


// Function to calculate CRC16
void calc_crc16(const uint8_t *data, size_t length, uint8_t result[2]) {
  DEBUGF("crc16 length=%d\n", length);
  unsigned short Init_CRC = 0x3AA3;

  for (size_t i = 0; i < length; i++) {
    Init_CRC = ((Init_CRC >> 8) & MASK_BIT_TYPE) ^ CRC16_Table[(Init_CRC ^ data[i]) % MAX_INT_8];
  }

  // Store CRC result in uint8_t array
  result[0] = Init_CRC & 0xFF;         // Low byte
  result[1] = (Init_CRC >> 8) & 0xFF;  // High byte
}

// Function to calculate CRC32
void calc_crc32(const uint8_t *data, size_t length, uint8_t result[4]) {
  unsigned int Init_CRC = 0x3AA3;

  for (size_t i = 0; i < length; i++) {
    unsigned char msg = 0x000000ff & data[i];
    unsigned int tmp = Init_CRC ^ msg;
    Init_CRC = (Init_CRC >> 8) ^ CRC32_Table[tmp & 0xff];
  }

  // Store CRC result in uint8_t array
  result[0] = Init_CRC & 0xFF;         // Least significant byte
  result[1] = (Init_CRC >> 8) & 0xFF;
  result[2] = (Init_CRC >> 16) & 0xFF;
  result[3] = (Init_CRC >> 24) & 0xFF; // Most significant byte
}

bool validate_data(uint8_t *data) {
  uint16_t expected_crc16, actual_crc16;
  uint32_t expected_crc32, actual_crc32;

  uint8_t crc16_result[2];
  uint8_t crc32_result[4];

  int crc16_index = 10;
  size_t data_len = size_t(*(uint16_t *)(data + 1));

  calc_crc16(data, crc16_index, crc16_result);

  expected_crc16 = *(uint16_t *)(crc16_result);
  actual_crc16 = *(uint16_t *)(data + crc16_index);

  if (expected_crc16 != actual_crc16) {
    WARNF("Invalid CRC16 checksum, expecting 0x%04X, got 0x%04X\n", expected_crc16, actual_crc16);
    //WARNF("Data:\n");
    //for (int i = 0; i < data_len; i++) {
    //  WARNF("%02X ", data[i]);
    //  if ((i + 1) % 8 == 0)  {
    //    WARNF("\n");
    //  }
    //}
    //WARNF("\n");
    return false;
  }

  int crc32_index = data_len - 4;
  calc_crc32(data, crc32_index, crc32_result);

  expected_crc32 = *(uint32_t *)(crc32_result);
  actual_crc32 = *(uint32_t *)(data + crc32_index);

  if (expected_crc32 != actual_crc32) {
    WARNF("Invalid CRC32 checksum, expecting 0x%08X, got 0x%08X\n", expected_crc32, actual_crc32);
    //WARNF("Data:\n");
    //for (int i = 0; i < data_len; i++) {
    //  WARNF("%02X ", data[i]);
    //  if ((i + 1) % 8 == 0)  {
    //    WARNF("\n");
    //  }
    //}
    //WARNF("\n");
    return false;
  }

  return true;
}

void parse_response_metadata(uint8_t *data, uint8_t *cmd_type, uint16_t *response_seq, uint8_t *cmd_set, uint8_t *cmd_id, uint8_t *return_code) {
  *cmd_type = data[3];
  *response_seq = *(uint16_t *)(data + 8);
  *cmd_set = data[12];
  *cmd_id = data[13];
  *return_code = data[14];
}

bool DJIRoninController::begin() {
  esp32Can.setPins(tx_pin, rx_pin);
  return esp32Can.begin(esp32Can.convertSpeed(rate_speed));
}

bool DJIRoninController::_send_data(uint8_t *data, size_t data_len) {
  bool ret = true;

  DEBUGF("Initial data len=%d\n", data_len);

  // Calculate the full frames and remaining data length
  size_t full_frame_num = data_len / FRAME_LEN;
  size_t left_len = data_len % FRAME_LEN;

  size_t frame_num = (left_len == 0) ? full_frame_num : full_frame_num + 1;

  DEBUGF("Frame number=%d\n", frame_num);

  CanFrame *frames = new CanFrame[frame_num];

  // Fill full frames
  for (size_t i = 0; i < full_frame_num; i++) {
    frames[i] = {0};
    frames[i].identifier = can_send_id;
    frames[i].extd = 0;
    frames[i].data_length_code = FRAME_LEN;
    for (size_t j = 0; j < FRAME_LEN; j++) {
      frames[i].data[j] = data[i*FRAME_LEN + j];
    }
  }

  // Handle the last frame if there's remaining data
  if (left_len > 0) {
    frames[frame_num - 1] = {0};
    frames[frame_num - 1].identifier = can_send_id;
    frames[frame_num - 1].extd = 0;
    frames[frame_num - 1].data_length_code = left_len;
    for (size_t j = 0; j < left_len; j++) {
      frames[frame_num - 1].data[j] = data[full_frame_num*FRAME_LEN + j];
    }
  }

  // Sending each frame
  for (size_t i = 0; i < frame_num; i++) {
    DEBUGF("Sending frame[%d]\n", i);
    DEBUGF("frame[%d].identifier=%02x\n", i, frames[i].identifier);
    DEBUGF("frame[%d].extd=%02x\n", i, frames[i].extd);
    DEBUGF("frame[%d].data_length_code=%02x\n", i, frames[i].data_length_code);
    //for (int j = 0; j < frames[i].data_length_code; j++) {
    //    DEBUGF("%02X ", frames[i].data[j]);
    //}
    //DEBUGF("\n");
    if (!esp32Can.writeFrame(frames[i])) {
      ret = false;
      WARNF("Failed to send frame[%d]\n", i);
    }
  }

  delete[] frames;
  return ret;
}

size_t DJIRoninController::_assemble_can_msg(uint8_t cmd_type, uint8_t cmd_set, uint8_t cmd_id, const uint8_t* data, size_t data_len, uint8_t *can_msg) {
  size_t idx = 0; // Index for writing to can_msg array

  // Fixed values
  uint8_t enc = 0;        // Encoding set to 0
  uint8_t res1 = 0;       // Reserved 1 set to 0
  uint8_t res2 = 0;       // Reserved 2 set to 0
  uint8_t res3 = 0;       // Reserved 3 set to 0
  uint16_t seq = _seq_num();

  // Assemble the header
  can_msg[idx++] = SOF_BYTE; // SOF byte (set to 0xAA)
  size_t cmd_length = data_len + 18; // data_len + cmd_set + cmd_id + other bits in the data packet.
  can_msg[idx++] = cmd_length & 0xFF;        // Low byte of length
  can_msg[idx++] = (cmd_length >> 8) & 0xFF; // High byte of length
  can_msg[idx++] = cmd_type; // Command Type
  can_msg[idx++] = enc; // Encoding
  can_msg[idx++] = res1; // Reserved 1 (set to 0)
  can_msg[idx++] = res2; // Reserved 2 (set to 0)
  can_msg[idx++] = res3; // Reserved 3 (set to 0)
  can_msg[idx++] = seq & 0xFF; // Low byte of seq.
  can_msg[idx++] = (seq >> 8) & 0xFF; // High byte of seq.

  // Calculate CRC16 and append to the header
  uint8_t crc16_result[2];
  calc_crc16(can_msg, idx, crc16_result);
  can_msg[idx++] = crc16_result[0];
  can_msg[idx++] = crc16_result[1];

  // Add the command data (cmd_set, cmd_id, data)
  can_msg[idx++] = cmd_set;
  can_msg[idx++] = cmd_id;
  for (size_t i = 0; i < data_len; ++i) {
    can_msg[idx++] = data[i];
  }

  // Calculate CRC32 and append it
  uint8_t crc32_result[4];
  calc_crc32(can_msg, idx, crc32_result);
  for (size_t i = 0; i < 4; ++i) {
    can_msg[idx++] = crc32_result[i];
  }
  return idx;
}

uint16_t DJIRoninController::_seq_num() {
  return ++seq_num;
}

bool DJIRoninController::_recv_data(uint8_t *data) {
  CanFrame frame;
  uint8_t data_buffer[DATA_BUFFER_SIZE];
  size_t data_offset = 0;
  bool seen_sof = false;


  while (true) {
    if (!esp32Can.readFrame(frame)) {
      DEBUGF("No frame received\n");
      break;
    }
    DEBUGF("Received frame: %02X, length=%d\n", frame.identifier, frame.data_length_code);

    if (frame.identifier != can_recv_id) {
      DEBUGF("Not an expected frame, skipping...\n");
      continue;
    }

    DEBUGF("Frame data:\n");
    for(int i = 0; i < frame.data_length_code; i++) {
      DEBUGF("%02X ", frame.data[i]);
    }
    DEBUGF("\n");

    if (frame.data[0] == SOF_BYTE) {
      seen_sof = true;
    }
    if (!seen_sof) {
      WARNF("Unexpected SOF header %02X, skipping...\n", frame.data[0]);
      continue;
    }
    if (data_offset + frame.data_length_code > DATA_BUFFER_SIZE) {
      WARNF("Not enough data buffer for a frame, try to increase the data buffer\n");
      return false;
    }

    memcpy(data_buffer+data_offset, frame.data, frame.data_length_code);
    data_offset += frame.data_length_code;

    if (data_offset >= 3) {
      size_t data_len = size_t(*(uint16_t *)(data_buffer + 1));
      if (data_offset >= data_len) {
        break; // Full message received.
      }
    }
  }

  if (!validate_data(data_buffer)) {
    return false;
  }

  memcpy(data, data_buffer, DATA_BUFFER_SIZE);
  return true;
}

bool DJIRoninController::_execute_command(uint8_t cmd_type, uint8_t cmd_set, uint8_t cmd_id, uint8_t *data, size_t data_len, uint8_t *recv_data) {
  uint8_t message[32];
  uint8_t recv_message[DATA_BUFFER_SIZE];

  size_t message_len = _assemble_can_msg(cmd_type, cmd_set, cmd_id, data, data_len, message);
  if (!_send_data(message, message_len)) {
    return false;
  }

  if (recv_data == NULL) {
    return true;
  }

  uint8_t return_cmd_type, return_cmd_set, return_cmd_id, return_code;
  uint16_t response_seq;
  int data_offset = 14;
  while (true) {
    if (!_recv_data(recv_message)) {
      return false;
    }
    parse_response_metadata(recv_message, &return_cmd_type, &response_seq, &return_cmd_set, &return_cmd_id, &return_code);
    if (cmd_type & 0x20 == 0) {
      WARNF("Invalid response type, not a reply frame, cmd_type=0x%02F\n", return_cmd_type);
      return false;
    }
    if (seq_num != response_seq) {
      WARNF("Invalid response SEQ, expect 0x%02X, got 0x%02X\n", seq_num, response_seq);
      return false;
    }
    if (return_cmd_set != cmd_set || return_cmd_id != cmd_id) {
      WARNF("Invalid cmd_set or cmd_id, expect cmd_set=0x%02X(got 0x%02X), cmd_id=0x%02X(got 0x%02X)\n", return_cmd_set, cmd_set, return_cmd_id, cmd_id);
      return false;
    }
    memcpy(recv_data, recv_message+data_offset, DATA_BUFFER_SIZE-data_offset);
    return_code = (uint8_t)(recv_data[0]);
    if (return_code != 0) {
      WARNF("Unexpected return_code, expect 0x00, got 0x%02X\n", return_code);
      return false;
    }
    // Valid response;
    break;
  }
  return true;
}


bool DJIRoninController::set_position(float yaw, float roll, float pitch, bool absolute_position, uint16_t time_for_action_in_millis = 1000) {
  uint16_t yaw_int = uint16_t(yaw * 10);
  uint16_t roll_int = uint16_t(roll * 10);
  uint16_t pitch_int = uint16_t(pitch * 10);
  uint8_t time_for_action_cnt = time_for_action_in_millis / 100;

  uint8_t data[8];
  size_t data_len = 0;

  uint8_t recv_data[DATA_BUFFER_SIZE];

  data[data_len++] = yaw_int & 0xFF;
  data[data_len++] = (yaw_int >> 8) & 0xFF;
  data[data_len++] = roll_int & 0xFF;
  data[data_len++] = (roll_int >> 8) & 0xFF;
  data[data_len++] = pitch_int & 0xFF;
  data[data_len++] = (pitch_int >> 8) & 0xFF;
  data[data_len++] = absolute_position ? 0x01 : 0x00;
  data[data_len++] = time_for_action_cnt;

  // By providing recv_data we force the _execute_command to check the return code.
  if (!_execute_command(0x02, 0x0e, 0x00, data, data_len, recv_data)) {
    WARNF("Failed to execute command\n");
    return false;
  }
  return true;
}

bool DJIRoninController::get_position(float *yaw, float *roll, float *pitch) {
  uint8_t send_data[1] = { 0x01 }; // 0x01 = attitude angle, 0x02 = joint angle
  uint8_t recv_data[DATA_BUFFER_SIZE];

  if (!_execute_command(0x02, 0x0e, 0x02, send_data, 1, recv_data)) {
    WARNF("Failed to execute command\n");
    return false;
  }

  uint8_t data_type = recv_data[1];
  if (data_type != 0x01) {
       WARNF("Unexpected data_type, expect 0x01, got 0x%02X\n", data_type);
     return false;
   }

  // Parse the yaw, roll, pitch value.
  int16_t yaw_res = *(int16_t *)(recv_data + 2);
  int16_t roll_res = *(int16_t *)(recv_data + 4);
  int16_t pitch_res = *(int16_t *)(recv_data + 6);

  *yaw = float(yaw_res) / 10.0;
  *roll = float(roll_res) / 10.0;
  *pitch = float(pitch_res) / 10.0;

  return true;
}

bool DJIRoninController::get_version(uint8_t version[4]) {
  uint32_t device_id = 0x00000001; // Device ID for the DJI R SDK.
  uint8_t recv_data[DATA_BUFFER_SIZE];

  if (!_execute_command(0x02, 0x0e, 0x09, (uint8_t *)(&device_id), 4, recv_data)) {
    WARNF("Failed to execute command\n");
    return false;
  }

  uint32_t reply_device_id = uint32_t(recv_data[1]);

  if (reply_device_id != device_id) {
    WARNF("Mismatching device ID, expect: 0x%08X, got: %08X\n", device_id, reply_device_id);
    return false;
  }
  version[0] = recv_data[8];
  version[1] = recv_data[7];
  version[2] = recv_data[6];
  version[3] = recv_data[5];
  return true;
}

bool DJIRoninController::gimbal_active_track() {
  uint8_t send_data[1] = { 0x03 };

  if (!_execute_command(0x02, 0x0e, 0x11, send_data, 1, NULL)) {
    WARNF("Failed to execute command\n");
    return false;
  }
  return true;
}

bool DJIRoninController::gimbal_recenter() {
  uint8_t send_data[2] = { 0xFE, 0x01 };
  uint8_t recv_data[DATA_BUFFER_SIZE];

  if (!_execute_command(0x02, 0x0e, 0x0e, send_data, 2, NULL)) {
    WARNF("Failed to execute command\n");
    return false;
  }
  return true;
}

bool DJIRoninController::start_camera_recording() {
  uint8_t send_data[2] = { 0x03, 0x00 };
  uint8_t recv_data[DATA_BUFFER_SIZE];

  // By providing recv_data we force the _execute_command to check the return code.
  if (!_execute_command(0x02, 0x0d, 0x00, send_data, 2, recv_data)) {
    WARNF("Failed to execute command\n");
    return false;
  }
  return true;
}

bool DJIRoninController::stop_camera_recording() {
  uint8_t send_data[2] = { 0x04, 0x00 };
  uint8_t recv_data[DATA_BUFFER_SIZE];

  // By providing recv_data we force the _execute_command to check the return code.
  if (!_execute_command(0x02, 0x0d, 0x00, send_data, 2, recv_data)) {
    WARNF("Failed to execute command\n");
    return false;
  }
  return true;
}

void DJIRoninController::dump_frame() {
  CanFrame frame;
  while (true) {
    if (!esp32Can.readFrame(frame)) {
      break;
    }
    DEBUGF("Received frame: %02X, length=%d\n", frame.identifier, frame.data_length_code);

    for(int i = 0; i < frame.data_length_code; i++) {
      DEBUGF("%02X ", frame.data[i]);
      if (i + 1 % 8 == 0) {
        DEBUGF("\n");
      }
    }
    DEBUGF("\n");
  }
}
