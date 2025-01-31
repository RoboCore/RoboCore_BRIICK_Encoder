/*******************************************************************************
* RoboCore BRIICK Encoder - Blink (v1.0)
* 
* Simple program to blink the LED of the BRIICK Encoder.
* 
* Copyright 2025 RoboCore.
* Written by Francois (23/10/2024).
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
#include <BRIICK_Encoder.h>

BRIICK_Encoder encoder(Wire);

const uint16_t DELAY_TIME = 1000; // [ms]

void setup() {
  // configure the I2C communication
  Wire.begin();
  
  // configure the serial communication
  Serial.begin(115200);

  // configure the BRIICK
  Serial.print("Encoder config: ");
  Serial.println(encoder.config());
}

void loop() {
  // blink LED
  encoder.setLED(HIGH);
  delay(DELAY_TIME);
  encoder.setLED(LOW);
  delay(DELAY_TIME);
}
