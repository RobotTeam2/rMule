/*
  SerialPassthrough sketch

  Some boards, like the Arduino 101, the MKR1000, Zero, or the Micro, have one
  hardware serial port attached to Digital pins 0-1, and a separate USB serial
  port attached to the IDE Serial Monitor. This means that the "serial
  passthrough" which is possible with the Arduino UNO (commonly used to interact
  with devices/shields that require configuration via serial AT commands) will
  not work by default.

  This sketch allows you to emulate the serial passthrough behaviour. Any text
  you type in the IDE Serial monitor will be written out to the serial port on
  Digital pins 0 and 1, and vice-versa.

  On the 101, MKR1000, Zero, and Micro, "Serial" refers to the USB Serial port
  attached to the Serial Monitor, and "Serial1" refers to the hardware serial
  port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  created 23 May 2016
  by Erik Nyquist
*/

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  Serial.print("arduino\r\n");
  Serial.print("serialsync\r\n");
}

static String gResponse_1 = "";
static String gResponse_2 = "";
static String gResponse_3 = "";
//static const String PackageSpace("&$");

bool isReadyToRelay(const String & response) {
  int lastIndex = response.length() -1;
  int preLastIndex = response.length() -2;
  if(preLastIndex >= 0) {
    if(response.charAt(lastIndex) == '$' && response.charAt(preLastIndex) == '&') {
      return true;
    }
  }
  if(lastIndex > 255) {
    return true;
  }
  return false;
}

void response(const String & response) {
  for(byte ch:response){
    Serial.write(ch);
  }
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    byte broadcast = Serial.read();
    //Serial.write(broadcast);
    Serial1.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
    Serial2.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
    Serial3.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    int incomming = Serial1.read();
    gResponse_1 += incomming;
    if(isReadyToRelay(gResponse_1)) {
      response(gResponse_1);
      gResponse_1 = "";
    }
  }
  if (Serial2.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    int incomming = Serial2.read();
    gResponse_2 += incomming;
    if(isReadyToRelay(gResponse_2)) {
      response(gResponse_2);
      gResponse_2 = "";
    }
  }
  if (Serial3.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    int incomming = Serial3.read();
    gResponse_3 += incomming;
    if(isReadyToRelay(gResponse_3)) {
      response(gResponse_3);
      gResponse_3 = "";
    }
  }
}
