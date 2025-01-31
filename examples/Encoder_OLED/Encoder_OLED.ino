/*******************************************************************************
* RoboCore BRIICK Encoder - Encoder (v1.0)
* 
* Simple program to read and print the position of the BRIICK Encoder.
* This program requires the use of the BRIICK OLED ( https://www.robocore.net/briick/briick-display-oled-0_96 ).
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
#include <SH1106Wire.h> // https://github.com/ThingPulse/esp8266-oled-ssd1306

SH1106Wire display(0x3C, SDA, SCL);

BRIICK_Encoder encoder(Wire);
int32_t encoder_position = 0;
const uint8_t STEPS_PER_REVOLUTION = 80;

const uint16_t DELAY_TIME = 250; // [ms]
uint32_t timeout = 0;

void setup() {
  // configure the I2C communication
  Wire.begin();
  
  // configure the serial communication
  Serial.begin(115200);

  // configure the BRIICK
  Serial.print("Encoder config: ");
  Serial.println(encoder.config());

  // configure the display
  display.init();
  display.flipScreenVertically();
}

void loop() {
  encoder_position = encoder.read();
  
  if (millis() > timeout){
    Serial.print("Current position: ");
    Serial.println(encoder_position);

    int8_t pos_fixed = constrain(encoder_position, (-1 * STEPS_PER_REVOLUTION), STEPS_PER_REVOLUTION);
    uint8_t pos_progress_bar = map(pos_fixed, (-1 * STEPS_PER_REVOLUTION), STEPS_PER_REVOLUTION, 0, 100);

    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(50, 20, String(pos_fixed));
    display.drawProgressBar(10, 50, 110, 10, pos_progress_bar);
    display.display();

    timeout = millis() + DELAY_TIME;
  }
}
