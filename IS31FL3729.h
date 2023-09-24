#ifndef __IS31FL3729__
#define __IS31FL3729__

#include <Wire.h>
#include "Arduino.h"

// IS31FL3729 I2C Address is determined by AD pin
#define IS31FL3729_ADDR_1 0x34 // AD pin is connected to GND
#define IS31FL3729_ADDR_2 0x35 // AD pin is connected to SCL
#define IS31FL3729_ADDR_3 0x36 // AD pin is connected to SDA
#define IS31FL3729_ADDR_4 0x37 // AD pin is connected to VCC

// Switch configuration settings determines how many switches (positive current) are enabled.
// There are 8 dedicated switchs, and the ninth switch can be repurposed to a current sink (CS16)
// The default config is SW_8SWx16CS
typedef enum _SWITCH_SETTING {
  SW_9SWx15CS = 0,  // 0000 n=9, SW1~SW9, 9SW×15CS matrix
  SW_8SWx16CS = 1,  // 0001 n=8, SW1~SW8, 8SW×16CS matrix
  SW_7SWx16CS = 2,  // 0010 n=7, SW1~SW7, 7SW×16CS matrix, SW8 no-active
  SW_6SWx16CS = 3,  // 0011 n=6, SW1~SW6, 6SW×16CS matrix, SW7~SW8 no-active
  SW_5SWx16CS = 4,  // 0100 n=5, SW1~SW5, 5SW×16CS matrix, SW6~SW8 no-active
  SW_4SWx16CS = 5,  // 0101 n=4, SW1~SW4, 4SW×16CS matrix, SW5~SW8 no-active
  SW_3SWx16CS = 6,  // 0110 n=3, SW1~SW3, 3SW×16CS matrix, SW4~SW8 no-active
  SW_2SWx16CS = 7,  // 0111 n=2, SW1~SW2, 2SW×16CS matrix, SW3~SW8 no-active
  SW_ALL_ON   = 8,  // 1000 SW1~SW9 with same phase, all on.
} SWITCH_SETTING;

// PWMF PWM frequency setting
typedef enum _PWM_FREQUENCY {
  FREQ_55khz  = 0, // 000 55kHz
  FREQ_32khz  = 1, // 001 32kHz
  FREQ_4khz   = 2, // 010 4kHz
  FREQ_2khz   = 3, // 011 2kHz
  FREQ_1khz   = 4, // 100 1kHz
  FREQ_500hz  = 5, // 101 0.5kHz, (n≤4)
  FREQ_250hz  = 6, // 110 0.25KHz, (n≤2)
  FREQ_80khz  = 7, // 111 80kHz
} PWM_FREQUENCY;

// Phase choice
typedef enum _PHASE_DELAY {
  DEGREE_0    = 0, // 0 degree phase delay
  DEGREE_180  = 1, // 180 degree phase delay
} PHASE_DELAY;

// Pull down Resistor Selection
typedef enum _PULL_DOWN_SETTING {
  PULL_DOWN_NONE = 0, // 000 No pull down resistor
  PULL_DOWN_500ohm_OFF = 1, // 001 0.5kΩ only in SWx off time
  PULL_DOWN_1kohm_OFF = 2, // 010 1.0kΩ only in SWx off time
  PULL_DOWN_2kohm_OFF = 3, // 011 2.0kΩ only in SWx off time
  PULL_DOWN_1kohm = 4, // 100 1.0kΩ all the time
  PULL_DOWN_2kohm = 5, // 101 2.0kΩ all the time
  PULL_DOWN_4kohm = 6, // 110 4.0kΩ all the time
  PULL_DOWN_8kohm = 7, // 111 8.0kΩ all the time
} PULL_DOWN_SETTING;

// Pull up Resistor Selection
typedef enum _PULL_UP_SETTING {
  PULL_UP_NONE = 0, // 000 No pull up resistor
  PULL_UP_500ohm_OFF = 1, // 001 0.5kΩ only in CSx off time
  PULL_UP_1kohm_OFF = 2, // 010 1.0kΩ only in CSx off time
  PULL_UP_2kohm_OFF = 3, // 011 2.0kΩ only in CSx off time
  PULL_UP_1kohm = 4, // 100 1.0kΩ all the time
  PULL_UP_2kohm = 5, // 101 2.0kΩ all the time
  PULL_UP_4kohm = 6, // 110 4.0kΩ all the time
  PULL_UP_8kohm = 7, // 111 8.0kΩ all the time
} PULL_UP_SETTING;
 
class IS31FL3729 {
private:
  TwoWire *_i2cPort;
	uint8_t _deviceAddress; // I2C Address of IS31FL3729
  int _shutdown_pin;
  SWITCH_SETTING _sws;
  
  void config_rmw(uint8_t val, uint8_t mask);
  bool readBytes(uint8_t firstRegisterAddress, uint8_t *destination, uint8_t length);
  bool readByte(uint8_t registerAddress, uint8_t *value);
  bool writeByte(uint8_t registerAddress, uint8_t writeValue);

public:
  IS31FL3729(int shutdown_pin = -1, SWITCH_SETTING sws = SW_8SWx16CS);
  uint8_t begin(uint8_t address = IS31FL3729_ADDR_1, TwoWire &wirePort = Wire, uint8_t global_current = 0x40);
  // Scaling controls the DC output current of each current sink CSy.
  // Each CSy has a byte to modulate the scaling in 256 steps. The value of the
  // Scaling decides the peak current of each LED.
  // cs values are 1..16 inclusive.
  bool set_current_sink_scale(uint8_t cs, uint8_t val);
  // Global Current modulates all CSy (y=1~16) DC current in 65 steps (maximum value is 64)
  // The actual current depends on the ISET resistor, the scaling value for each current sink, and the LED pwm.
  bool set_global_current(uint8_t val);
  // Set the PWM frequency, default is 32kHz. In order to avoid LED display flicker,
  // it is recommended PWM frequency ÷ n (number of switches) is higher than 100Hz, so when PWM
  // frequency is 0.5kHz, n cannot be more than 4, when PWM frequency is 0.25kHz, n cannot be more than 2. 
  bool set_pwm_freq(PWM_FREQUENCY freq);
  // Pull Down/Up Resistor Selection. The default is 0 degree phase, and 2.0kΩ pull up and down
  // resistors (only when CSx/SWx are off)
  bool set_pull_up_down(PHASE_DELAY phc, PULL_DOWN_SETTING sw_pd, PULL_UP_SETTING cs_pu);
  // Each led has a byte to modulate the PWM duty in 256 steps. This function identifies a led
  // by its direct led address (from 0x01 to 0x8F)
  bool set_led(uint8_t led, uint8_t val);
  // Each led has a byte to modulate the PWM duty in 256 steps. This function identifies a led
  // by sw/cs coordinates. cs values are 1..16 inclusive. and sw values are 1..9 inclusive.
  bool set_led(uint8_t sw, uint8_t cs, uint8_t val);
  // During hardware shutdown the state Function Registers can be
  // accessed but all analog circuits are disabled to conserve power.
  // This function will use a shudown pin if available otherwise enter a soft shutdown.
  bool shutdown();
  // Resume back from shutdown, and enable normal mode.
  bool resume();
  // Reset all registers to their default values (a blank display).
  bool reset();
};

#endif