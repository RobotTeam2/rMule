
#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">&$");\
  Serial.print("\r\n");\
}


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
  //Serial.print(response);
  int lastIndex = response.length() -1;
  int preLastIndex = response.length() -2;
  if(preLastIndex >= 0) {
    //Serial.write(response.charAt(lastIndex));
    //Serial.print("\r\n");
    if(response.charAt(lastIndex) == '$' && response.charAt(preLastIndex) == '&') {
      //Serial.write(true);
      return true;
    }
    if(response.charAt(lastIndex) == '\n' && response.charAt(preLastIndex) == '\r') {
      //Serial.write(true);
      return true;
    }
  }
  if(lastIndex > 255) {
    //Serial.write(true);
    return true;
  }
  //Serial.write(false);
  return false;
}

void response(const String & response) {
  Serial.print(response);
  /*
  for(byte ch:response){
    Serial.write(ch);
  }
  */
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    char broadcast = Serial.read();
    //Serial.write(broadcast);
    Serial1.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
    //Serial2.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
    //Serial3.write(broadcast);   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    char incomming = Serial1.read();
    Serial.write(incomming);
    gResponse_1 += incomming;
    bool ready = isReadyToRelay(gResponse_1);
    //DUMP_VAR(ready);
    if(ready) {
      response(gResponse_1);
      gResponse_1 = "";
    }
  }
  if (Serial2.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    //Serial.write(Serial2.peek());
    char incomming = Serial2.read();
    gResponse_2 += incomming;
    bool ready = isReadyToRelay(gResponse_2);
    //DUMP_VAR(ready);
    if(ready) {
      response(gResponse_2);
      gResponse_2 = "";
    }
  }
  if (Serial3.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    //Serial.write(Serial3.peek());
    char incomming = Serial3.read();
    gResponse_3 += incomming;
    bool ready = isReadyToRelay(gResponse_3);
    //DUMP_VAR(ready);
    if(ready) {
      response(gResponse_3);
      gResponse_3 = "";
    }
  }
  //Serial.print("\r\n");
}
