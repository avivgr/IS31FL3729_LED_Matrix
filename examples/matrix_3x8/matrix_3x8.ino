#include "IS31FL3729.h"

IS31FL3729 leds(10, SW_3SWx16CS);

void setup() {
  Serial.begin(115200);
  Serial.println("IS31FL3729");
  Wire.begin();
  leds.begin();

  for(int i = 1; i <= 8; i++) {
    leds.set_current_sink_scale(i, 0xff);
  }
}

void loop() {
  static uint8_t n = 0;
  static uint8_t v = 0x50;

  for(int i = 1; i <= 3; i++) {
    leds.set_led(i, n + 1, v);
  }
  
  delay(400);

  for(int i = 1; i <= 3; i++) {
    leds.set_led(i, n + 1, 0);
  }

  n = (n + 1) % 8;
}
