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

// --------------------------------------------------
// Libraries

#include <BRIICK_Encoder.h>

// --------------------------------------------------
// --------------------------------------------------

// Constructor (default)
//  @param (i2c) : the I2C/TWI bus used for communication [TwoWire]
BRIICK_Encoder::BRIICK_Encoder(TwoWire & i2c) :
  BRIICK_Encoder(i2c, ENCODER_ADDRESS) // call the main constructor
{
  // nothing to do
}

// --------------------------------------------------

// Constructor
//  @param (i2c) : the I2C/TWI bus used for communication [TwoWire]
//  @param (address) : the I2C address of the device [uint8_t]
BRIICK_Encoder::BRIICK_Encoder(TwoWire & i2c, uint8_t address) :
#ifdef BRIICK_ENCODER_DEBUG
  _debugger(nullptr),
#endif
  _i2c(nullptr),
  _address(address),
  _initialized(false),
  _buttonState(0),
  _encoderPosition(0),
  _encoderState(0xFF),
  _ledAttached(false),
  _ledState(0)
{
  this->_i2c = &i2c;
}

// --------------------------------------------------
// --------------------------------------------------

// Attach the LED to the button
//  returns true if successful [bool]
bool BRIICK_Encoder::attachLED(void) {
  // check if initialized
  if (!this->_initialized) {
    return false;
  }

  this->_ledAttached = true;

  return true;
}

// --------------------------------------------------

// Configure the BRIICK
//  @param (force) : true to force the initialization [bool]
//  @returns true if successful [bool]
//  Note: the I2C/TWI bus must be initialized before configuring the object.
//  Note: the force feature shouldn't be necessary, but it might be helpful in some cases.
bool BRIICK_Encoder::config(bool force) {
  // check if already initialized
  if (this->_initialized) {
    if (force) {
      this->_initialized = false; // reset
    } else {
      return true;
    }
  }
  
  // Note (for future reference)
  // <Wire.begin()> could be used to make sure the I2C communication is
  // configured and active, but it might be problematic if several calls
  // to <Wire.begin()> would occur in the same program, as it resets
  // the driver. Instead, the library uses a reference to the bus and
  // tries to communicate over it as master.

  // configure the GPIO
  uint8_t register_mask = (0xFF ^ ENCODER_LED); // button and encoder as inputs and LED a output (a little different than ENCODER_INPUT_ALL)
  this->_i2c->beginTransmission(this->_address);
  this->_i2c->write(ENCODER_REGISTER_CONFIG);
  this->_i2c->write(register_mask);
  if (this->_i2c->endTransmission() == 0) {
    this->_initialized = true; // set
    
    // turn off the LEDs
    this->setLED(LOW);
  } else {
#ifdef BRIICK_ENCODER_DEBUG
    if (this->_debugger != nullptr) {
      this->_debugger->println("Couldn't access the encoder");
    }
#endif
  }

  return this->_initialized;
}

// --------------------------------------------------

// Detach the LED from the button
//  returns true if successful [bool]
bool BRIICK_Encoder::detachLED(void) {
  // check if initialized
  if (!this->_initialized) {
    return false;
  }

  this->_ledAttached = false;

  return true;
}

// --------------------------------------------------

// Read the encoder
//  @returns the current position [uint8_t] (0 if not initialized)
int32_t BRIICK_Encoder::read(void){
  // check if initialized
  if (!this->_initialized) {
    return 0;
  }

  // get the data
  this->_data = ENCODER_ERROR; // reset
  this->_i2c->beginTransmission(this->_address);
  this->_i2c->write(ENCODER_REGISTER_INPUT);
  if (this->_i2c->endTransmission() == 0) {
    if (this->_i2c->requestFrom(this->_address, static_cast<uint8_t>(1)) > 0) {
      this->_data = this->_i2c->read();
      this->_data &= ENCODER_INPUT_ALL; // mask the inputs
    }
  }


// Encoder logic based on Paul Stoffregen's work ( https://github.com/PaulStoffregen/Encoder )
//                           _______         _______       
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

//    new       old
//  p2  p1    p2  p1    result
//  --------------------------
//  0   0     0   0     no movement
//  0   0     0   1     +1
//  0   0     1   0     -1
//  0   0     1   1     +2 (assume edge detection on pin 1)

//  0   1     0   0     -1
//  0   1     0   1     no movement
//  0   1     1   0     -2 (assume edge detection on pin 1)
//  0   1     1   1     +1

//  1   0     0   0     +1
//  1   0     0   1     -2 (assume edge detection on pin 1)
//  1   0     1   0     no movement
//  1   0     1   1     -1

//  1   1     0   0     +2 (assume edge detection on pin 1)
//  1   1     0   1     -1
//  1   1     1   0     +1
//  1   1     1   1     no movement


  // handle the data
  if (this->_data != ENCODER_ERROR) {
    if (this->_encoderState == 0xFF){
      this->_encoderState = (this->_data & ENCODER_CHANNEL_A)? 0x04 : 0x00;
      this->_encoderState |= (this->_data & ENCODER_CHANNEL_B)? 0x08 : 0x00;
    } else {
      this->_encoderState >>= 2; // shift
      this->_encoderState |= (this->_data & ENCODER_CHANNEL_A)? 0x04 : 0x00;
      this->_encoderState |= (this->_data & ENCODER_CHANNEL_B)? 0x08 : 0x00;

      switch(this->_encoderState){
        case 0b0000:
        case 0b0101:
        case 0b1010:
        case 0b1111:
          // nothing to do
          break;
        
        case 0b0001:
        case 0b0111:
        case 0b1000:
        case 0b1110:
          this->_encoderPosition += 1;
          break;
        
        case 0b0010:
        case 0b0100:
        case 0b1011:
        case 0b1101:
          this->_encoderPosition -= 1;
          break;
        
        case 0b0011:
        case 0b1100:
          this->_encoderPosition += 2;
          break;
        
        case 0b0110:
        case 0b1001:
          this->_encoderPosition -= 2;
          break;
        
        default:
          // nothing to do (shouldn't enter here)
          break;
      }
    }
  } else {
#ifdef BRIICK_ENCODER_DEBUG
    if (this->_debugger != nullptr) {
      this->_debugger->println("Couldn't read the encoder");
    }
#endif
  }

  return this->_encoderPosition;
}

// --------------------------------------------------

// Read the button
//  @returns the state of the button or ENCODER_ERROR on error [uint8_t]
uint8_t BRIICK_Encoder::readButton(void) {
  // check if initialized
  if (!this->_initialized) {
    return ENCODER_ERROR;
  }

  // get the data
  this->_data = ENCODER_ERROR; // reset (redundant, but safer)
  this->read(); // use the same I2C read to update the encoder

  // handle the data
  if (this->_data != ENCODER_ERROR) {
    this->_buttonState = (this->_data & ENCODER_BUTTON) ?  HIGH : LOW; // update

    // udpate the attached LEDs
    this->_updateLED();

    return this->_buttonState;
  } else {
#ifdef BRIICK_ENCODER_DEBUG
    if (this->_debugger != nullptr) {
      this->_debugger->println("Couldn't read the encoder");
    }
#endif
  }

  return this->_data;
}

// --------------------------------------------------

// Set the debugger of the object
//  @param (stream) : the stream to print to [Stream &]
#ifdef BRIICK_ENCODER_DEBUG
void BRIICK_Encoder::setDebugger(Stream & stream) {
  this->_debugger = &stream;
}
#endif // BRIICK_ENCODER_DEBUG

// --------------------------------------------------

// Set the state of the LED
//  @param (level) : the level to set (0 or 1) [uint8_t]
//  returns true if successful [bool]
bool BRIICK_Encoder::setLED(uint8_t level) {
  // check if initialized
  if (!this->_initialized) {
    return false;
  }

  // check the parameter
  if (level > HIGH) {
    return false;
  }

  // check if attached
  if (this->_ledAttached) {
    // don't update an attached LED
    return false;
  }
  // set the data to be sent
  uint8_t mask = 0;
  if (level == HIGH){
    mask = ENCODER_LED;
  }

  // send the data
  this->_i2c->beginTransmission(this->_address);
  this->_i2c->write(ENCODER_REGISTER_OUTPUT);
  this->_i2c->write(mask);
  if (this->_i2c->endTransmission() == 0) {
    this->_ledState = level; // update
    return true;
  } else {
#ifdef BRIICK_ENCODER_DEBUG
    if (this->_debugger != nullptr) {
      this->_debugger->println("Couldn't write to the encoder");
    }
#endif
  }

  return false;
}

// --------------------------------------------------

// Update the state of the attached LED
//  returns true if successful [bool]
bool BRIICK_Encoder::_updateLED(void) {
  // check if initialized
  if (!this->_initialized) {
    return false;
  }

  // check if there are LED attached
  if (!this->_ledAttached) {
    return true;
  }

  // set the data to be sent
  uint8_t mask = 0;
  if (this->_buttonState == HIGH){
    mask = ENCODER_LED;
  }

  // send the data
  this->_i2c->beginTransmission(this->_address);
  this->_i2c->write(ENCODER_REGISTER_OUTPUT);
  this->_i2c->write(mask);
  if (this->_i2c->endTransmission() == 0) {
    this->_ledState = this->_buttonState; // update
    return true;
  } else {
#ifdef BRIICK_ENCODER_DEBUG
    if (this->_debugger != nullptr) {
      this->_debugger->println("Couldn't write to the encoder");
    }
#endif
  }

  return false;
}

// --------------------------------------------------
