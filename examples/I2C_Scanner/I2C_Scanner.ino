/*******************************************************************************
* RoboCore BRIICK Encoder - I2C Scanner (v1.0)
* 
* Simple I2C scanner.
* 
* Copyright 2025 RoboCore.
* Written by Francois (28/06/2024).
* Based on several examples on the internet.
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

#include <Wire.h>

void setup() {
  // configure the serial communication
  Serial.begin(115200);
  Serial.println("I2C Scanner");

  // configure the I2C communication
  Wire.begin();
}

void loop() {
  Serial.print("I2C addresses found:");
  for (uint8_t address = 1 ; address < 127 ; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf(" 0x%X", address);
    }
    delay(50);
  }
  Serial.println();

  delay(1000);
}
