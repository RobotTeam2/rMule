#include <EEPROM.h>

#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">\n");\
}

const static char MOTER_PWM_WHEEL = 5;
const static char MOTER_CCW_WHEEL = 4;
// Interrupt
const static char MOTER_FGS_WHEEL = 2;
const static char MOTER_VOLUME_WHEEL = A1;

const static char MOTER_PWM_LINEAR = 11;
const static char MOTER_STANDBY_LINEAR = 9;
const static char MOTER_A1_LINEAR = 10;
const static char MOTER_A2_LINEAR = 12;
const static char MOTER_CURRENT_LINEAR = A6;



void setup()
{
  pinMode(MOTER_PWM_WHEEL, OUTPUT);
  pinMode(MOTER_CCW_WHEEL, OUTPUT);
  pinMode(MOTER_VOLUME_WHEEL, INPUT_PULLUP);
  pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL),CounterWheelFGSByInterrupt , FALLING);

  //TCCR1B = (TCCR1B & 0b11111000) | 0x01;
  //TCCR2B &= B11111000;
  //TCCR2B |= B00000001;
  // pwm 5pin
  //TCCR0B = (TCCR0B & 0b11111000) | 0x01;
  analogWrite(MOTER_PWM_WHEEL, 0x0);
  
  pinMode(MOTER_PWM_LINEAR, OUTPUT);
  pinMode(MOTER_STANDBY_LINEAR, OUTPUT);
  pinMode(MOTER_A1_LINEAR, OUTPUT);
  pinMode(MOTER_A2_LINEAR, OUTPUT);
  pinMode(MOTER_CURRENT_LINEAR, INPUT_PULLUP);
  

  digitalWrite(MOTER_PWM_LINEAR, HIGH);
  digitalWrite(MOTER_STANDBY_LINEAR, HIGH);
  digitalWrite(MOTER_A1_LINEAR, HIGH);
  digitalWrite(MOTER_A2_LINEAR, HIGH);



  //Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg\n");\

  loadEROM();

}

void loop() {
  checkOverRunLimit();
  runSerialCommand();
  readStatus();
  calcWheelTarget();
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
  speed_wheel = 0x0;\
  analogWrite(MOTER_PWM_WHEEL, 0x0);\
}

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

static String gSerialInputCommand = "";
void runSerialCommand(void) {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    //Serial.print(incomingByte);
    if(incomingByte =='\n' || incomingByte =='\r') {
      run_comand();
      gSerialInputCommand = "";
    } else {
      gSerialInputCommand += incomingByte;
    }
    if(gSerialInputCommand.length() > 128) {
      gSerialInputCommand = "";
    }
  }
}


void responseTextTag(String &res) {
  Serial.print(res);
}
void run_simple_command(void) {
  if(gSerialInputCommand=="uu") {
    speed_wheel -= 5;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="dd") {
    speed_wheel += 5;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="ff") {
    FRONT_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="bb") {
    BACK_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="ss") {
    speed_wheel =0xff;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="gg") {
    speed_wheel =0;
    analogWrite(MOTER_CCW_WHEEL, speed_wheel);  
  }
}


void run_comand(void) {
  //DUMP_VAR(InputCommand);
  //DUMP_VAR(speed_wheel);
  run_simple_command();
  
  if(gSerialInputCommand.startsWith("info:") || gSerialInputCommand.startsWith("I:")) {
    runInfo();
  }
  if(gSerialInputCommand.startsWith("setting:") || gSerialInputCommand.startsWith("S:")) {
    runSetting();
  }

  if(gSerialInputCommand.startsWith("gpio:") || gSerialInputCommand.startsWith("G:")) {
    runGPIO();
  }
  if(gSerialInputCommand.startsWith("wheel:") || gSerialInputCommand.startsWith("W:")) {
    runWheel();
  }


}
void runInfo(void) {
  String resTex;
  resTex += "info:id,";
  resTex += String(iEROMLegId);      
  resTex += ":lb,";
  resTex += String(iEROMWheelLimitBack);      
  resTex += ":lf,";
  resTex += String(iEROMWheelLimitFront);
  resTex += "\n";
  responseTextTag(resTex);
}

void runSetting(void) {
  int firstId = gSerialInputCommand.indexOf(":id,");
  //DUMP_VAR(firstId);
  if(firstId > 0) {
    String legStr = gSerialInputCommand.substring(firstId+4);
    //DUMP_VAR(legStr);
    int legID = legStr.toInt();
    //DUMP_VAR(legID);
    saveEROM(iEROMLegIdAddress,legID);
    iEROMLegId = legID;
  }
  int firstLF = gSerialInputCommand.indexOf(":lf,");
  //DUMP_VAR(firstLF);
  if(firstLF > 0) {
    String lfStr = gSerialInputCommand.substring(firstLF+4);
    //DUMP_VAR(lfStr);
    int limitFront = lfStr.toInt();
    //DUMP_VAR(limitFront);
    saveEROM(iEROMWheelLimitFrontAddress,limitFront);
    iEROMWheelLimitFront = limitFront;
  }
  int firstLB = gSerialInputCommand.indexOf(":lb,");
  //DUMP_VAR(firstLB);
  if(firstLB > 0) {
    String lbStr = gSerialInputCommand.substring(firstLB+4);
    //DUMP_VAR(lbStr);
    int limitBack = lbStr.toInt();
    //DUMP_VAR(limitBack);
    saveEROM(iEROMWheelLimitBackAddress,limitBack);
    iEROMWheelLimitBack = limitBack;
  }

}


void runGPIO(void) {
  //DUMP_VAR(gSerialInputCommand);
  int first = gSerialInputCommand.indexOf(":");
  int last = gSerialInputCommand.indexOf(",");
  if(first < 0 && last > gSerialInputCommand.length() -1) {
    return;
  }
  
  String portStr = gSerialInputCommand.substring(first+1,last);
  //DUMP_VAR(portStr);
  int port  = portStr.toInt();
  //DUMP_VAR(port);
  String valStr = gSerialInputCommand.substring(last);
  int val  = valStr.toInt();
  //DUMP_VAR(val);
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
void runWheel(void) {
  //DUMP_VAR(gSerialInputCommand);
  int first = gSerialInputCommand.indexOf(":");
  int last = gSerialInputCommand.lastIndexOf(":");
  if(first < 0 && last > gSerialInputCommand.length() -1) {
    return;
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
  //DUMP_VAR(iTargetDistanceWheelFGS);
  if(iTargetDistanceWheelFGS <= 0) {
    STOP_WHEEL();
    bIsRunWheelByFGS = false;
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


  if(iTargetDistanceWheelFGS < 0) {
    bIsRunWheelByFGS = not bIsRunWheelByFGS;
    iTargetDistanceWheelFGS = abs(iTargetDistanceWheelFGS);
  }
  
  
}
