#include "motor_control.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// PWM / speeds
#define PWM_FREQ     5000
#define PWM_RES      8
#define DUTY_STOP    0
#define DUTY_70      178
#define DUTY_60      178

struct MotorConfig {
  uint8_t in1_pin;
  uint8_t in2_pin;
  int pwm_channel_in1;
  int pwm_channel_in2;
  volatile int current_speed;
  volatile bool direction_forward;
};

static MotorConfig motor1 = {IN1, IN2, 0, 1, DUTY_STOP, true};
static MotorConfig motor2 = {IN3, IN4, 2, 3, DUTY_STOP, true};

static void motorTask(void *pvParameters) {
  MotorConfig *motor = (MotorConfig *)pvParameters;

  pinMode(motor->in1_pin, OUTPUT);
  pinMode(motor->in2_pin, OUTPUT);
  digitalWrite(motor->in1_pin, LOW);
  digitalWrite(motor->in2_pin, LOW);

  ledcSetup(motor->pwm_channel_in1, PWM_FREQ, PWM_RES);
  ledcAttachPin(motor->in1_pin, motor->pwm_channel_in1);
  ledcSetup(motor->pwm_channel_in2, PWM_FREQ, PWM_RES);
  ledcAttachPin(motor->in2_pin, motor->pwm_channel_in2);

  for (;;) {
    if (motor->direction_forward) {
      ledcWrite(motor->pwm_channel_in1, motor->current_speed);
      ledcWrite(motor->pwm_channel_in2, DUTY_STOP);
    } else {
      ledcWrite(motor->pwm_channel_in1, DUTY_STOP);
      ledcWrite(motor->pwm_channel_in2, motor->current_speed);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

namespace Motor {

  void begin() {
    Serial.println("Motor: inicializando...");
    // asegurar stop
    motor1.current_speed = DUTY_STOP;
    motor1.direction_forward = true;
    motor2.current_speed = DUTY_STOP;
    motor2.direction_forward = true;

    // crear tareas (cada motor en su core)
    xTaskCreatePinnedToCore(motorTask, "Motor1Task", 2048, &motor1, 5, NULL, 0);
    xTaskCreatePinnedToCore(motorTask, "Motor2Task", 2048, &motor2, 5, NULL, 1);

    delay(50);
    stop();
  }

  void forward() {
    motor1.direction_forward = true;
    motor2.direction_forward = true;
    motor1.current_speed = DUTY_70;
    motor2.current_speed = DUTY_70;
    Serial.println("Motor: forward");
  }

  void back() {
    motor1.direction_forward = false;
    motor2.direction_forward = false;
    motor1.current_speed = DUTY_70;
    motor2.current_speed = DUTY_70;
    Serial.println("Motor: back");
  }

  void left() {
    motor1.direction_forward = true;
    motor1.current_speed = DUTY_60;
    motor2.direction_forward = false;
    motor2.current_speed = DUTY_60;
    Serial.println("Motor: left");
  }

  void right() {
    motor1.direction_forward = false;
    motor1.current_speed = DUTY_60;
    motor2.direction_forward = true;
    motor2.current_speed = DUTY_60;
    Serial.println("Motor: right");
  }

  void stop() {
    motor1.current_speed = DUTY_STOP;
    motor2.current_speed = DUTY_STOP;
    Serial.println("Motor: stop");
  }

  void setMotorSpeed(int motorNum, int speed, bool forward) {
    if (motorNum == 1) {
      motor1.current_speed = constrain(speed, 0, 75);
      motor1.direction_forward = forward;
    } else if (motorNum == 2) {
      motor2.current_speed = constrain(speed, 0, 75);
      motor2.direction_forward = forward;
    }
  }
}
