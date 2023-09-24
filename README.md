# IS31FL3729_LED_Matrix
Arduino library for [IS31FL3729](https://lumissil.com/assets/pdf/core/IS31FL3729_DS.pdf) LED Matrix from Lumissil Microsystems.

## Overview 

The IS31FL3729 is a general purpose 16×8 (or 15×9) matrix LED driver programmed via an I2C bus
interface. Each LED can be dimmed individually with 8-bit PWM data, and each current sink has
8-bit DC current scaling for color calibration. The combination enables 256 steps of linear PWM dimming
for each dot and 256 steps of DC current adjustment for each current sink.

IS31FL3729 Features:
- Supply voltage range: 2.7V to 5.5V
- 16 current sinks and 8 switches
- Support 16×n (n=1~8), 15×9 LED matrix configurations
- Individual 256 PWM control steps
- 256 DC current steps for each current sink
- 64 global current steps
- Shutdown pin - rising edge resets the I2C interface
- Register programmable PWM frequency: 0.25kHz to 55kHz
- 1MHz I2C-compatible interface
- Individual open and short error detect function
- PWM 180 degree phase shift
- Spread spectrum
- De-ghost
- QFN-32 (4mm×4mm) and eTQFP-32 packages 
