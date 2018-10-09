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

 FluidLevel.h
 Created on: Oct 9, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#ifndef SRC_FLUIDLEVEL_H_
#define SRC_FLUIDLEVEL_H_

#include <Arduino.h>
#include <CapacitiveSensor.h>

#define CAP_LEVEL_TRIGGER 200

class FluidLevel {
public:
  ~FluidLevel() {
    if (sensor != nullptr) {
      delete sensor;
    }
  }

  void begin() {
    if (sensor != nullptr) {
      delete sensor;
    }
    sensor = new CapacitiveSensor(CAP_PIN_SEND, CAP_PIN_RECIEVE);
  }

  String getStatus() {
    if (sensor != nullptr) {
      long total = sensor->capacitiveSensor(30);
      bool trigger = total > CAP_LEVEL_TRIGGER;
      String result(F("{\"level\":"));
      result += total;
      result += F(", \"trigger\":");
      result += trigger;
      result += F("}");

      digitalWrite(LED_BUILTIN, trigger ? HIGH : LOW);
      return result;

    } else {
      return F("{}");
    }
  }
private:
  CapacitiveSensor* sensor = nullptr;
};



#endif /* SRC_FLUIDLEVEL_H_ */
