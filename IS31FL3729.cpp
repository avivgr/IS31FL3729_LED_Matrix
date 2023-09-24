#include "IS31FL3729.h"

// https://lumissil.com/assets/pdf/core/IS31FL3729_DS.pdf
#define REG_LED_PWM(_led) (0x01 + _led) // [01h-8Fh] set pwm value for led
#define REG_SCALING(_cs) (0x90 + _cs) // [90h-9Fh] control the dc output current for each Current Sink
#define REG_CONFIG 0xA0 // A0h Configure operation mode
#define REG_GCC 0xA1 // A1h Global Current Control register
#define REG_PULL_CFG 0xB0 // B0h Pull Down/Up Resistor Selection Register (set the pulldown resistor for SWx and pull up resistor for CSy)
#define REG_SPREAD_SPECT 0xB1 // B1h Spread Spectrum Register
#define REG_PWM_FREQ 0xB2 // B2h PWM Frequency Register
#define REG_OPEN_SHORT(_n) (0xB3 + _n) // [B3h-C4h] Open/Short Register Store the open or short information
#define REG_RESET 0xCF // CFh Reset Register

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

IS31FL3729::IS31FL3729(int shutdown_pin, SWITCH_SETTING sws)
{
  _shutdown_pin = shutdown_pin;
  _sws = sws;
}

bool IS31FL3729::shutdown()
{
  /* if there is a shutdown pin, use hardware shutdown */
  if(_shutdown_pin != -1) {
    digitalWrite(_shutdown_pin, LOW);
  } else {
    /* software shutdown */
    config_rmw(0, 1);
  }

  return true;
}

bool IS31FL3729::resume()
{
  /* if there is a shutdown pin, use hardware shutdown */
  if(_shutdown_pin != -1) {
    digitalWrite(_shutdown_pin, HIGH);
    delay(10);
  }
  /* exit software shutdown - into normal mode */
  config_rmw(1, 1);

  return true;
}

bool IS31FL3729::reset()
{
  return writeByte(REG_RESET, 0xAE);
}

void IS31FL3729::config_rmw(uint8_t newval, uint8_t mask)
{
  uint8_t val = 0;

  readByte(REG_CONFIG, &val);
  val = (val & ~mask) | (newval & mask);
  writeByte(REG_CONFIG, val);
}

uint8_t IS31FL3729::begin(uint8_t address, TwoWire &wirePort, uint8_t global_current)
{
  uint8_t val = 0;

  _i2cPort = &wirePort;
  _deviceAddress = address;

  if(_shutdown_pin != -1) {
    pinMode(_shutdown_pin, OUTPUT);
  }

  resume();
  reset();

  // readByte(REG_CONFIG, &val); Serial.println(val);
  config_rmw(_sws << 4 | 1, 0xF1);
  set_global_current(global_current);
  // readByte(REG_CONFIG, &val); Serial.println(val);

  return 0;
}

bool IS31FL3729::set_pwm_freq(PWM_FREQUENCY freq)
{
  return writeByte(REG_PWM_FREQ, freq);
}

bool IS31FL3729::set_global_current(uint8_t val)
{
  return writeByte(REG_GCC, val);
}

bool IS31FL3729::set_current_sink_scale(uint8_t cs, uint8_t val)
{
  // 16 current sinks
  if(cs < 1 || cs > 16) {
    return false;
  }
  return writeByte(REG_SCALING(cs-1), val);
}

bool IS31FL3729::set_pull_up_down(PHASE_DELAY phc, PULL_DOWN_SETTING sw_pd, PULL_UP_SETTING cs_pu)
{
  uint8_t val = (phc << 7) | (sw_pd << 4) | cs_pu;

  return writeByte(REG_PULL_CFG, val);
}

bool IS31FL3729::readBytes(uint8_t firstRegisterAddress, uint8_t *destination, uint8_t length)
{
	_i2cPort->beginTransmission(_deviceAddress);
	_i2cPort->write(firstRegisterAddress);
	uint8_t endResult = _i2cPort->endTransmission();
	bool result = (endResult == 0) && (_i2cPort->requestFrom(_deviceAddress, length) == length);

	if (result)	{
		for (uint8_t i = 0; i < length; i++) {
			destination[i] = _i2cPort->read();
		}
	}
	return result;
}

bool IS31FL3729::readByte(uint8_t registerAddress, uint8_t *value)
{
  return readBytes(registerAddress, value, 1);
}

bool IS31FL3729::writeByte(uint8_t registerAddress, uint8_t writeValue)
{
	_i2cPort->beginTransmission(_deviceAddress);
	bool result = _i2cPort->write(registerAddress) && _i2cPort->write(writeValue);
	uint8_t endResult = _i2cPort->endTransmission();
	return result && (endResult == 0);
}

bool IS31FL3729::set_led(uint8_t led, uint8_t val)
{
  if(led < 1 || led > 0x8F) {
    return false;
  }
  return writeByte(led, val);
}

bool IS31FL3729::set_led(uint8_t sw, uint8_t cs, uint8_t val)
{
  uint8_t led = (sw - 1) * 0x10 + cs;

  if((sw < 1 || sw > 9) || (cs < 1 || cs > 16) || led > 0x8F) {
    return false;
  }
  return writeByte(led, val);
}
