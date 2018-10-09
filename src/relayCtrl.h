/*
 BSD 3-Clause License

 Copyright (c) 2017, The Tosters
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 relayCtrl.h
 Created on: Oct 9, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#ifndef SRC_RELAYCTRL_H_
#define SRC_RELAYCTRL_H_

#include <Arduino.h>

class RelayCtrl {
public:
  void begin() {
    for(int t = 0; t < 2; t++) {
      pinMode(pins[t], OUTPUT);
    }
  }

  void update() {
    const unsigned long mil = millis();
    for(int t = 0; t < 2; t++) {
      if ((timeTofinish[t] > 0) && (mil > timeTofinish[t])) {
        digitalWrite(pins[t], LOW);
        timeTofinish[t] = 0;
      }
    }
  }

  void switchOn(uint8_t index) {
    if (index >= 2) {
      return;
    }
    digitalWrite(pins[index], HIGH);
    timeTofinish[index] = millis() + timeToHold[index];
  }

private:
  unsigned long timeTofinish[2] = {0, 0};
  uint8_t pins[2] = {RELAY_1_PIN, RELAY_2_PIN};
  int timeToHold[2] = {RELAY_1_TIME, RELAY_2_TIME};
};

#endif /* SRC_RELAYCTRL_H_ */
