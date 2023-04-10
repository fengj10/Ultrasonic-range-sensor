#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "lib/lcd/lcd.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

uint trigPin = 8;
uint echoPin = 9;
uint buzzerPin = 7;

int timeout = 26100;
int distence = 0;

int main(void)
{
  //Setup
  LCD_Setup();
  stdio_init_all();
  setupUltrasonicPins(trigPin, echoPin, buzzerPin);

  while (1) {
    LCDWriteStringXY(0, 0, "Distance: ");
    distence =  getCm(trigPin, echoPin);
    LCDWriteStringXY(0, 1, distence);

    if (distence < 20 && distence > 10) {
      beep(buzzerPin);
    }
    if (distence < 10) {
      beep1(buzzerPin);
    }
  }
}

void setupUltrasonicPins(uint trigPin, uint echoPin, uint buzzerPin)
{
  gpio_init(trigPin);
  gpio_init(echoPin);
  gpio_init(buzzerPin);
  gpio_set_dir(trigPin, GPIO_OUT);
  gpio_set_dir(echoPin, GPIO_IN);
  gpio_set_dir(buzzerPin, GPIO_OUT);
}

void beep(uint buzzerPin)
{
  gpio_put(buzzerPin, 1);
  _delay_ms(500);
  gpio_put(buzzerPin, 0);
  _delay_ms(500);
}

void beep1(uint buzzerPin)
{
  gpio_put(buzzerPin, 1);
  _delay_ms(100);
  gpio_put(buzzerPin, 0);
  _delay_ms(100);
}

uint64_t getPulse(uint trigPin, uint echoPin)
{
  gpio_put(trigPin, 1);
  sleep_us(10);
  gpio_put(trigPin, 0);

  uint64_t width = 0;

  while (gpio_get(echoPin) == 0) {
    tight_loop_contents();
  }
  absolute_time_t startTime = get_absolute_time();
  while (gpio_get(echoPin) == 1)
  {
    width++;
    sleep_us(1);
    if (width > timeout) {
      return 0;
    }
  }
  absolute_time_t endTime = get_absolute_time();

  return absolute_time_diff_us(startTime, endTime);
}

uint64_t getCm(uint trigPin, uint echoPin)
{
  uint64_t pulseLength = getPulse(trigPin, echoPin);
  return pulseLength / 29 / 2;
}
