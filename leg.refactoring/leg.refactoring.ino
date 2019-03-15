#include <ArduinoJson.h>
#include <EEPROM.h>

typedef StaticJsonDocument<128> MyJsonDoc;

#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  Serial.print(x);\
  Serial.print(">\n");\
}


const static char MOTER_PWM_WHEEL = 5;
const static char MOTER_CCW_WHEEL = 4;
// Interrupt
const static char MOTER_FGS_WHEEL = 2;
const static char MOTER_VOLUME_WHEEL = A1;



const static char MOTER_CURRENT_LINEAR = A6;

const static char MOTER_PWM_LINEAR = 11;
const static char MOTER_STANDBY_LINEAR = 9;
const static char MOTER_A1_LINEAR = 10;
const static char MOTER_A2_LINEAR = 12;



unsigned char speed_wheel = 0x0;

static long wheelRunCounter = -1;
static long const iRunTimeoutCounter = 10000L * 10L;



#define FRONT_WHEEL() { \
  digitalWrite(MOTER_CCW_WHEEL, LOW);\
  }


#define BACK_WHEEL() { \
  digitalWrite(MOTER_CCW_WHEEL, HIGH);\
  }

#define STOP_WHEEL() {\
  speed_wheel =0x00;\
  analogWrite(MOTER_PWM_WHEEL, 0x0);\
}



const int  iEROMLegIdAddress = 0;
const int  iEROMWheelLimitBackAddress = iEROMLegIdAddress + 2; 
const int  iEROMWheelLimitFrontAddress = iEROMWheelLimitBackAddress + 4; 

uint16_t  iEROMLegId = 0;
uint16_t  iEROMWheelLimitBack = 280; 
uint16_t  iEROMWheelLimitFront = 420; 

void loadEROM() {
  DUMP_VAR(EEPROM.length());
  {
    byte value1 = EEPROM.read(iEROMLegId);
    byte value2 = EEPROM.read(iEROMLegId+1);
    iEROMLegId = value1 | value2 << 8;
    DUMP_VAR(iEROMLegId);
  }
  {
    byte value1 = EEPROM.read(iEROMWheelLimitBackAddress);
    byte value2 = EEPROM.read(iEROMWheelLimitBackAddress+1);
    iEROMWheelLimitBack = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelLimitBack);
  }
  {
    byte value1 = EEPROM.read(iEROMWheelLimitFrontAddress);
    byte value2 = EEPROM.read(iEROMWheelLimitFrontAddress+1);
    iEROMWheelLimitFront = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelLimitFront);
  }
}
void saveEROM(int address,uint16_t value) {
  byte value1 =  value & 0xff;
  EEPROM.write(address,value1);
  byte value2 = (value >> 8) & 0xff;
  EEPROM.write(address+1,value2);
}


void repsponseJson(const MyJsonDoc &doc) {
  String output;
  serializeJson(doc, output);
  Serial.print(output);
  Serial.print("\n");
}




void setup()
{
  pinMode(MOTER_VOLUME_WHEEL, INPUT_PULLUP);
  
  pinMode(MOTER_PWM_WHEEL, OUTPUT);
  pinMode(MOTER_CCW_WHEEL, OUTPUT);

  //pinMode(MOTER_FGS_WHEEL, INPUT);
  //pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL),CounterWheelFGSByInterrupt , FALLING);
  
  

  pinMode(MOTER_CURRENT_LINEAR, INPUT_PULLUP);
  
  pinMode(MOTER_PWM_LINEAR, OUTPUT);
  pinMode(MOTER_STANDBY_LINEAR, OUTPUT);
  pinMode(MOTER_A1_LINEAR, OUTPUT);
  pinMode(MOTER_A2_LINEAR, OUTPUT);
  

  digitalWrite(MOTER_PWM_LINEAR, HIGH);
  digitalWrite(MOTER_STANDBY_LINEAR, HIGH);
  digitalWrite(MOTER_A1_LINEAR, HIGH);
  digitalWrite(MOTER_A2_LINEAR, HIGH);


  //TCCR1B = (TCCR1B & 0b11111000) | 0x01;
  //TCCR2B &= B11111000;
  //TCCR2B |= B00000001;
  // pwm 5pin
  //TCCR0B = (TCCR0B & 0b11111000) | 0x01;

  
  STOP_WHEEL();

  //Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg\n");\

}

String InputCommand ="";




int runMotorFGSignlCouter = 0;
int runMotorFGSignlCouter_NOT = 0;


void runWheel(int spd,int front) {
  speed_wheel = spd;
  analogWrite(MOTER_PWM_WHEEL, spd);
  wheelRunCounter = iRunTimeoutCounter;
  runMotorFGSignlCouter = 0;
  runMotorFGSignlCouter_NOT = 0;
  if(front) {
    digitalWrite(MOTER_CCW_WHEEL , HIGH);
    //DUMP_VAR(front);
  } else {
    digitalWrite(MOTER_CCW_WHEEL, LOW);
    //DUMP_VAR(front);
  }
}





void runLinear(int distance,int ground) {
 if(distance == 0) {
    digitalWrite(MOTER_A1_LINEAR, HIGH);
    digitalWrite(MOTER_A2_LINEAR, HIGH);
    return;
 }
 if(ground) {
    digitalWrite(MOTER_A1_LINEAR, LOW);
    digitalWrite(MOTER_A2_LINEAR, HIGH);
    DUMP_VAR(distance);
  } else {
    digitalWrite(MOTER_A1_LINEAR, HIGH);
    digitalWrite(MOTER_A2_LINEAR, LOW);
    DUMP_VAR(distance);
  }
}


void tryConfirmJson() {
  MyJsonDoc jsonBuffer;
  //DUMP_VAR(InputCommand);
  auto errorDS = deserializeJson(jsonBuffer,InputCommand);
  if (errorDS) {
    //DUMP_VAR(errorDS == DeserializationError::InvalidInput);
    //DUMP_VAR(errorDS == DeserializationError::NoMemory);
    DUMP_VAR(InputCommand);
    return;
  }
  JsonObject root =  jsonBuffer.as<JsonObject>();
  for (auto kv : root) {
    JsonObject params = kv.value();
    String command = kv.key().c_str();
    DUMP_VAR(command);
    if(command == "wheel" || command == "W") {
      int spd = params["s"];
      int front = params["f"];
      DUMP_VAR(spd);
      DUMP_VAR(front);
      runWheel(spd,front);
    }
    if(command == "linear" || command == "L") {
      int distance = params["d"];
      int ground = params["g"];
      DUMP_VAR(distance);
      DUMP_VAR(ground);
      runLinear(distance,ground);
    }

    if(command == "fgs" || command == "F") {
      int fgsWheelDistance = -1;
      if(params.containsKey("wheel")) {
        fgsWheelDistance = params["wheel"];
      }
      if(params.containsKey("W")) {
        fgsWheelDistance = params["W"];
      }
      if(fgsWheelDistance > 0) {
        runWheelFgs(fgsWheelDistance);
      }
    }

    if(command == "vol" || command == "V") {
      int volumeWheelDistance = -1;
      if(params.containsKey("wheel")) {
        volumeWheelDistance = params["wheel"];
      }
      if(params.containsKey("W")) {
        volumeWheelDistance = params["W"];
      }
      if(volumeWheelDistance > 0) {
        runWheelVolume(volumeWheelDistance);
      }
    }

    if(command == "gpio" || command == "G") {
      DUMP_VAR(command);
      for (auto kvG : params) {
        JsonObject paramsG = kvG.value();
        String portStr = kvG.key().c_str();
        int port = portStr.toInt();
        int value = kvG.value().as<int>();
        DUMP_VAR(port);
        DUMP_VAR(value);
        if(port > 1 && port < 14) {
          if(value == 0) {
            digitalWrite(port,LOW);
          }
          if(value == 1) {
            digitalWrite(port,HIGH);
          }
        }
        if(port > 0xA0 && port < 0xA8) {
          if(value == 0) {
            int rValue = analogRead(port);
            MyJsonDoc docRes;
            JsonObject rootRes = docRes.to<JsonObject>();
            rootRes[portStr] = rValue;
            repsponseJson(docRes);
          } else {
            analogWrite(port,value);
          }
        }
      }
    }
    
    
    if(command == "setting") {
      if(params.containsKey("leg")) {
        iEROMLegId = params["leg"];
        saveEROM(iEROMLegIdAddress,iEROMLegId);
      }
      if(params.containsKey("WheelLimitBack")) {
        iEROMWheelLimitBack = params["WheelLimitBack"];
        saveEROM(iEROMWheelLimitBackAddress,iEROMWheelLimitBack);
      }
      if(params.containsKey("WheelLimitFront")) {
        iEROMWheelLimitFront = params["WheelLimitFront"];
        saveEROM(iEROMWheelLimitFrontAddress,iEROMWheelLimitFront);
      }
    }
    if(command == "info") {
      MyJsonDoc docRes;
      JsonObject rootRes = docRes.to<JsonObject>();
      rootRes["leg"] = iEROMLegId;
      rootRes["lb"] = iEROMWheelLimitBack;
      rootRes["lf"] = iEROMWheelLimitFront;
      repsponseJson(docRes);
    }
  }
  //DUMP_VAR(command);
  
  {
    repsponseJson(jsonBuffer);
  }
  
  //DUMP_VAR(InputCommand);
  InputCommand = "";
}

void run_comand() {
  //DUMP_VAR(InputCommand);
  //DUMP_VAR(speed_wheel);
  if(InputCommand=="uu") {
    speed_wheel -= 5;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(InputCommand=="dd") {
    speed_wheel += 5;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(InputCommand=="ff") {
    FRONT_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(InputCommand=="bb") {
    BACK_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(InputCommand=="ss") {
    speed_wheel =0xff;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(InputCommand=="gg") {
    speed_wheel =0;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(InputCommand.startsWith("gpio")) {
    //DUMP_VAR(InputCommand);
    int first = InputCommand.indexOf(":");
    int last = InputCommand.lastIndexOf(":");
    if(first > 0 && last < InputCommand.length()) {
      String portStr = InputCommand.substring(first+1,last);
      //DUMP_VAR(portStr);
      int port  = portStr.toInt();
      //DUMP_VAR(port);
      String valStr = InputCommand.substring(last);
      int val  = valStr.toInt();
      //DUMP_VAR(val);
      runGPIO(port,val);
    }
  }
}

void responseTextTag(String &res) {
  Serial.print(res);
}

void runGPIO(int port,int val) {
  if(port > 2 && port < 14) {
    if(val == 0) {
      digitalWrite(port,LOW);
    } else {
      digitalWrite(port,HIGH);        
    }
  }
  if(port > 0xA0 && port < 0xA7) {
    if(val == 0) {
      int valRes = analogRead(port);
      //DUMP_VAR(valRes);
      String resTex;
      resTex += "gpio:";
      resTex += String(port,HEX);      
      resTex += ":";
      resTex += String(valRes);      
      resTex += "\n";
      responseTextTag(resTex);
    } else {
      analogWrite(port,val);        
    }
  }
}


void readStatus() {
  int current = analogRead(MOTER_CURRENT_LINEAR);
  if(abs(current - 506) > 10) {
    //DUMP_VAR(current);
  }
  readWheelVolume();
}

int iVolumeDistanceWheel = 0;
bool bIsRunWheelByVolume = false;


void checkOverRunLimit(void) {
  // stop
  if(wheelRunCounter-- <= 0 ) {
    STOP_WHEEL();
  }
  if(iVolumeDistanceWheel < iEROMWheelLimitBack) {
    bIsRunWheelByVolume = false;
    STOP_WHEEL();
  }
  if(iVolumeDistanceWheel > iEROMWheelLimitFront) {
    bIsRunWheelByVolume = false;
    STOP_WHEEL();
  }  
}

void runSerialCommand(void) {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    //Serial.print(incomingByte);
    if(incomingByte =='\n' || incomingByte =='\r') {
      run_comand();
      InputCommand = "";
    } else if(incomingByte =='}') {
      InputCommand += incomingByte;
      tryConfirmJson();
    } else {
      InputCommand += incomingByte;
    }
    if(InputCommand.length() > 128) {
      InputCommand = "";
    }
  }
}

void loop() {
  checkOverRunLimit();
  runSerialCommand();
  readStatus();
  //calcWheel2TargetFGS();
  calcWheelTarget();
}

int const iTargetDistanceMaxDiff = 1;


int iTargetDistanceWheelFGS = 0;
void CounterWheelFGSByInterrupt(void) {
  //DUMP_VAR(++runMotorFGSignlCouter);
  iTargetDistanceWheelFGS--;
  calcWheel2TargetFGS();
}


const int iConstVolumeDistanceWheelReportDiff = 2;
int iVolumeDistanceWheelReported = 0;

void readWheelVolume() {
  int volume = analogRead(MOTER_VOLUME_WHEEL);
  if(volume > 900) {
    return;
  }
  bool iReport = abs(volume - iVolumeDistanceWheelReported) > iConstVolumeDistanceWheelReportDiff;
  //DUMP_VAR(volume);
  //DUMP_VAR(abs(volume - iVolumeDistanceWheelReported));
  //bool iReport = true;
  if(iReport) {
    iVolumeDistanceWheelReported = volume;
    MyJsonDoc doc;
    JsonObject root = doc.to<JsonObject>();
    root["volume"] = volume;
    repsponseJson(doc);
  }
  iVolumeDistanceWheel = volume;
}
int iTargetVolumePostionWheel = 0;
void runWheelVolume(int distPostion) {
  if(distPostion < iEROMWheelLimitBack || distPostion > iEROMWheelLimitFront) {
    //DUMP_VAR(distPostion);
    //DUMP_VAR(iEROMWheelLimitBack);
    //DUMP_VAR(iEROMWheelLimitFront);
    return;
  }
  iTargetVolumePostionWheel = distPostion;
  bIsRunWheelByVolume = true;
  
  int moveDiff = iTargetVolumePostionWheel - iVolumeDistanceWheel;
  bool bForwardRunWheel = false;
  if(moveDiff > 0) {
    bForwardRunWheel = true;
  }
  //DUMP_VAR(bForwardRunWheel);
  if(bForwardRunWheel) {
    runWheel(254,1);
  } else {
    runWheel(254,0);
  }

  {
    MyJsonDoc doc;
    JsonObject root = doc.to<JsonObject>();
    root["distPostion"] = distPostion;
    root["iVolumeDistanceWheel"] = iVolumeDistanceWheel;
    root["bForwardRunWheel"] = bForwardRunWheel;
    root["iTargetVolumePostionWheel"] = iTargetVolumePostionWheel;
    repsponseJson(doc);
  }

}

/*
int const aVolumeSpeedTable[] = {
  0,  0,125,125,125,
  125,125,125,125,125,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  130,130,130,130,130,
  130,130,130,130,130,
  254
};
*/

int const aVolumeSpeedTable[] = {
  0,  0,125,125,125,
  125,125,125,125,125,
/*
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  130,130,130,130,130,
  130,130,130,130,130,
*/
  254
};


long const aVolumeSpeedTableLength = sizeof(aVolumeSpeedTable)/sizeof(aVolumeSpeedTable[0]);

int const iConstVolumeWheelNearTarget = 3;

void calcWheelTarget() {
  if(bIsRunWheelByVolume == false) {
    return;
  }
  int moveDiff = iTargetVolumePostionWheel - iVolumeDistanceWheel;
  int distanceToMove = abs(moveDiff);
  if(distanceToMove < iConstVolumeWheelNearTarget) {
    bIsRunWheelByVolume = false;
    STOP_WHEEL();
    {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["moveDiff"] = moveDiff;
      root["bIsRunWheelByVolume"] = bIsRunWheelByVolume;
      repsponseJson(doc);
    }
    return;
  } /*else {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["moveDiff"] = moveDiff;
      root["bIsRunWheelByVolume"] = bIsRunWheelByVolume;
      repsponseJson(doc);
  }*/
  
  bool bForwardRunWheel = false;
  if(moveDiff > 0) {
    bForwardRunWheel = true;
  }
  DUMP_VAR(bForwardRunWheel);
  int speedIndex = distanceToMove;
  if(distanceToMove >= aVolumeSpeedTableLength) {
    speedIndex = aVolumeSpeedTableLength -1;
  }
  int speed = aVolumeSpeedTable[speedIndex];
  if(bForwardRunWheel) {
    runWheel(speed,1);
  } else {
    runWheel(speed,0);
  }
}





bool bIsRunWheelByFGS = false;
bool bForwardRunWheelByFGS = false;
// 10mm per  signal 
float iTargetDistanceWheelFactorFGS = 0.90f;
void runWheelFgs(int distPostion) {
  if(distPostion < iEROMWheelLimitBack || distPostion > iEROMWheelLimitFront) {
    DUMP_VAR(distPostion);
    DUMP_VAR(iEROMWheelLimitBack);
    DUMP_VAR(iEROMWheelLimitFront);
    return;
  }
  int moveDiff = distPostion - iVolumeDistanceWheel;
  if(moveDiff > 0) {
    bForwardRunWheelByFGS = false;
  } else {
    bForwardRunWheelByFGS = true;
  }
  DUMP_VAR(bForwardRunWheelByFGS);
  float moveDistance = abs(moveDiff);
  iTargetDistanceWheelFGS = int (((float)moveDistance)/iTargetDistanceWheelFactorFGS);
  DUMP_VAR(iTargetDistanceWheelFGS);
  bIsRunWheelByFGS = true;
  if(bForwardRunWheelByFGS) {
    runWheel(254,1);
  } else {
    runWheel(254,0);
  }

  {
    MyJsonDoc doc;
    JsonObject root = doc.to<JsonObject>();
    root["distPostion"] = distPostion;
    root["iVolumeDistanceWheel"] = iVolumeDistanceWheel;
    root["bIsRunWheelByFGS"] = bIsRunWheelByFGS;
    root["iTargetDistanceWheelFGS"] = iTargetDistanceWheelFGS;
    root["moveDiff"] = moveDiff;
    root["moveDistance"] = moveDistance;
    repsponseJson(doc);
  }

}


int const aFGSSpeedTable[] = {
  0,  125,125,125,125,
/*
  125,125,125,125,125,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  130,130,130,130,130,
  130,130,130,130,130,
*/
  254
};
long const aFGSSpeedTableLength = sizeof(aFGSSpeedTable)/sizeof(aFGSSpeedTable[0]);



void calcWheel2TargetFGS() {
  if(bIsRunWheelByFGS == false) {
    return;
  }
  {
    MyJsonDoc doc;
    JsonObject root = doc.to<JsonObject>();
    root["iTargetDistanceWheelFGS"] = iTargetDistanceWheelFGS;
    repsponseJson(doc);
  }
  //DUMP_VAR(iTargetDistanceWheelFGS);
  if(iTargetDistanceWheelFGS <= 0) {
    STOP_WHEEL();
    bIsRunWheelByFGS = false;
    {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["bForwardRunWheelByFGS"] = bForwardRunWheelByFGS;
      root["iTargetDistanceWheelFGS"] = iTargetDistanceWheelFGS;
      repsponseJson(doc);
    }
    return;
  }
  /*
  if(iTargetDistanceWheelFGS < 0) {
    bIsRunWheelByFGS = not bIsRunWheelByFGS;
    iTargetDistanceWheelFGS = abs(iTargetDistanceWheelFGS);
    {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["bForwardRunWheelByFGS"] = bForwardRunWheelByFGS;
      root["iTargetDistanceWheelFGS"] = iTargetDistanceWheelFGS;
      repsponseJson(doc);
    }
  }
  */
  
  long moveIndex1 = iTargetDistanceWheelFGS;
  long moveIndex2 = moveIndex1;
  if(moveIndex1 >= aFGSSpeedTableLength) {
    moveIndex2 = aFGSSpeedTableLength -1;
  }
  
  //DUMP_VAR(iTargetDistanceWheelFGS);
  int speed = aFGSSpeedTable[moveIndex2];
  //DUMP_VAR(bForwardRunWheelByFGS);
  if(bForwardRunWheelByFGS) {
    runWheel(speed,1);
  } else {
    runWheel(speed,0);
  }

  {
    MyJsonDoc doc;
    JsonObject root = doc.to<JsonObject>();
    root["moveIndex1"] = moveIndex1;
    root["moveIndex2"] = moveIndex2;
    root["aFGSSpeedTableLength"] = aFGSSpeedTableLength;
    root["speed"] = speed;
    repsponseJson(doc);
  }

  if(iTargetDistanceWheelFGS < 0) {
    bIsRunWheelByFGS = not bIsRunWheelByFGS;
    iTargetDistanceWheelFGS = abs(iTargetDistanceWheelFGS);
    {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["moveIndex2"] = moveIndex2;
      root["speed"] = speed;
      repsponseJson(doc);
    }
  }
  
  
}
