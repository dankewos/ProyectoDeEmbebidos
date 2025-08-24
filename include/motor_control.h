#pragma once
#include <Arduino.h>

// Pines del L298N conectados a la ESP32-CAM
// (tal como confirmaste)
static const uint8_t IN1 = 13;
static const uint8_t IN2 = 15;
static const uint8_t IN3 = 4;
static const uint8_t IN4 = 14;

namespace Motor {
  void begin();
  void forward();
  void back();
  void left();
  void right();
  void stop();
  void setMotorSpeed(int motorNum, int speed, bool forward);
}
