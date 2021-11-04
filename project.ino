#include <BleKeyboard.h>

// This library is for using BLE HID with your ESP32
// 
// This is not availble on the library manager so
// needs to be installed from Github
// https://github.com/T-vK/ESP32-BLE-Keyboard

// First param is name
// Second is manufacturer
// Third is initial batter level

bool activeListener = false;
int count = 0;
unsigned long hex_num;


BleKeyboard bleKeyboard("BLE Terminal v4 B", "Estera", 100);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 22, 0);
  Serial2.begin(9600, SERIAL_8N1, 25, 33, 1);
  Serial.println("Begun");
  bleKeyboard.begin();
}

void loop() {
  // Only do something with the keypress if we
  // are connected to something via bluetooth
  if (bleKeyboard.isConnected()) {
    checkRFID(); 
    while(Serial2.available() > 0) {
      bleKeyboard.write(Serial2.read());
    }
  }
}

void checkRFID() {
  if (Serial1.available()) {      // If anything comes in Serial (USB),
    byte b = Serial1.read();
    Serial.print("Byte: ");
    Serial.println(b);
    
    
    if(activeListener == false && b == 0x02) {
      activeListener = true;
      count = 0;
    }

    if(activeListener == true) {
      count++;
      if(count > 3 && count < 8) {
        if(count == 4) {
          hex_num = b << 24;
        }
        if(count == 5) {
          hex_num += b << 16;
        }
        if(count == 6) {
          hex_num += b << 8;
        }
        if(count == 7) {
          hex_num += b;
        }
      }
    }
    
    if( b == 0x03 ) {
      activeListener = false;
      Serial.print("Sending to Bluetooth: ");
      Serial.println(hex_num);
      bleKeyboard.println(hex_num);
    }
  }
}
