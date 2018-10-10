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
#include <Arduino.h>
#include <relayCtrl.h>
#include <FluidLevel.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Console.h>
#include <sha256.h>
#include <FileIO.h>

RelayCtrl relayCtrl;
FluidLevel fluidLevel;
BridgeServer server;

uint8_t* key = nullptr;
int keyLen = 0;

void loadKey();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  relayCtrl.begin();
  fluidLevel.begin();
  Bridge.begin();
  server.begin();
  Console.begin();
  digitalWrite(LED_BUILTIN, LOW);
  loadKey();
}

void loadKey() {
  //to generate key, type in openwrt console
  //head -c 32 </dev/urandom >/etc/config/httpKey
  //
  //to dump in hex form:
  //hexdump /etc/config/httpKey
  FileSystem.begin();
  File keyFile = FileSystem.open("/etc/config/httpKey", FILE_READ);
  keyLen = keyFile.size();
  key = static_cast<uint8_t*>(malloc(keyLen));
  keyFile.read(key, keyLen);
  keyFile.close();
}

String findArg(String rawData, String argName) {
  argName.concat("=");
  int index = rawData.indexOf(argName);
  if (index < 0) {
    return "";
  }
  index += argName.length();
  int index2 = rawData.indexOf('&', index);
  index2 = index2 < 0 ? rawData.length() : index2;
  return rawData.substring(index, index2);
}

String extractCommand(String rawData) {
  rawData.trim();
  String remoteHMac = findArg(rawData, F("HMac"));
  String nonce = findArg(rawData, F("nonce"));
  String cmd = findArg(rawData, F("cmd"));
  Sha256.initHmac(key, keyLen);
  Sha256.print(nonce);
  Sha256.print("cmd");
  Sha256.print(cmd);
  Sha256.print(nonce);

  String hmac;
  hmac.reserve(64);
  uint8_t* hash = Sha256.resultHmac();
  for (int t = 0; t < 32; t++) {
    hmac.concat( (char)(65 + (hash[t]>>4)));
    hmac.concat( (char)(65 + (hash[t]&0xF)));
  }
//--------- Just for debug
  Console.print("cmd:");
  Console.println(cmd);
  Console.print("key:");
  for (int i = 0; i < keyLen; i++) {
    const byte nib1 = (key[i] >> 4) & 0x0F;
    const byte nib2 = (key[i] >> 0) & 0x0F;
    Console.print((char)(nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA));
    Console.print((char)(nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA));
  }
  Console.print(" [LEN=");
  Console.print(keyLen);
  Console.println(']');
  Console.print("nonce:");
  Console.println(nonce);
  Console.print("    my hmac:");
  Console.println(hmac);
  Console.print("remote hmac:");
  Console.println(remoteHMac);
//-----------^^
  return hmac.compareTo(remoteHMac) == 0 ? cmd : "";
}

void process(BridgeClient& client) {
  String cmd = extractCommand(client.readString());
  if (cmd == F("level")) {
    client.println(fluidLevel.getStatus());

  } else if (cmd == F("gate")) {
    relayCtrl.switchOn(1);
    client.println(F("200: OK"));

  } else if (cmd == F("door")) {
    relayCtrl.switchOn(0);
    client.println(F("200: OK"));
  }
}

void loop() {

  BridgeClient client = server.accept();

  if (client) {
    process(client);
    client.stop();
  }
  relayCtrl.update();
  delay(50);
}
