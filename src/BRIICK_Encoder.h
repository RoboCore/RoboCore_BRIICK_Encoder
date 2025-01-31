#ifndef BRIICK_ENCODER_H
#define BRIICK_ENCODER_H

/*******************************************************************************
* RoboCore BRIICK Encoder Library (v1.0)
* 
* Library to use the BRIICK Encoder v1.0.
* 
* Copyright 2025 RoboCore.
* 
* 
* This file is part of the BRIICK Encoder library by RoboCore ("RoboCore-BRIICK-Encoder-lib").
* 
* "RoboCore-BRIICK-Encoder-lib" is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* "RoboCore-BRIICK-Encoder-lib" is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public License
* along with "RoboCore-BRIICK-Encoder-lib". If not, see <https://www.gnu.org/licenses/>
*******************************************************************************/

#define BRIICK_ENCODER_DEBUG

// --------------------------------------------------
// Libraries

#include <stdint.h>

#include <Wire.h>

#ifdef BRIICK_ENCODER_DEBUG
#include <Stream.h>
#endif

// --------------------------------------------------
// Macros

#ifndef HIGH
#define HIGH (1)
#define LOW (0)
#endif

#define ENCODER_ADDRESS (0x23) // default address

#define ENCODER_REGISTER_INPUT   (0x00)
#define ENCODER_REGISTER_OUTPUT  (0x01)
#define ENCODER_REGISTER_CONFIG  (0x03)

#define ENCODER_ERROR   (0x81)

#define ENCODER_CHANNEL_A   (0x02)
#define ENCODER_CHANNEL_B   (0x01)
#define ENCODER_BUTTON      (0x04)
#define ENCODER_LED         (0x08)

#define ENCODER_INPUT_ALL  (ENCODER_CHANNEL_A | \
                             ENCODER_CHANNEL_B | \
                             ENCODER_BUTTON)
// #define ENCODER_INPUT_ALL  (0xFF ^ ENCODER_LED)

// --------------------------------------------------
// Class - BRIICK Encoder

class BRIICK_Encoder {
  public:
    BRIICK_Encoder(TwoWire &);
    BRIICK_Encoder(TwoWire &, uint8_t);

    bool attachLED(void);
    bool config(bool = false);
    bool detachLED(void);
    int32_t read(void);
    uint8_t readButton(void);
    bool setLED(uint8_t);

#ifdef BRIICK_ENCODER_DEBUG
    void setDebugger(Stream &);
#endif
  
  private:
    TwoWire * _i2c;
    uint8_t _address;
    uint8_t _buttonState;
    uint8_t _data;
    int32_t _encoderPosition;
    uint8_t _encoderState;
    bool _initialized;
    bool _ledAttached;
    uint8_t _ledState;

#ifdef BRIICK_ENCODER_DEBUG
    Stream * _debugger;
#endif

    bool _updateLED(void);
};

// --------------------------------------------------

#endif // BRIICK_ENCODER_H
