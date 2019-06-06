#include <EEPROM.h>

#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">&$");\
}

// Interrupt
const static char A_MOTER_FGS_WHEEL = 2;
const static char A_MOTER_PWM_WHEEL = 9;
const static char A_MOTER_CCW_WHEEL = 4;
const static char A_MOTER_VOLUME_WHEEL = A1;

const static char B_MOTER_FGS_WHEEL = 3;
const static char B_MOTER_PWM_WHEEL = 10;
const static char B_MOTER_CCW_WHEEL = 8;
const static char B_MOTER_VOLUME_WHEEL = A2;



void setup()
{
  // set pwm 9,10
  TCCR1B &= B11111000;
  TCCR1B |= B00000001;

  pinMode(A_MOTER_CCW_WHEEL, OUTPUT);
  pinMode(A_MOTER_FGS_WHEEL, INPUT_PULLUP);
  pinMode(A_MOTER_PWM_WHEEL, OUTPUT);
  pinMode(A_MOTER_VOLUME_WHEEL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(A_MOTER_FGS_WHEEL),A_Motor_FGS_By_Interrupt , FALLING);
  analogWrite(A_MOTER_PWM_WHEEL, 0);


  pinMode(B_MOTER_CCW_WHEEL, OUTPUT);
  pinMode(B_MOTER_FGS_WHEEL, INPUT_PULLUP);
  pinMode(B_MOTER_PWM_WHEEL, OUTPUT);
  pinMode(B_MOTER_VOLUME_WHEEL, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(B_MOTER_FGS_WHEEL),B_Motor_FGS_By_Interrupt , FALLING);
  analogWrite(B_MOTER_PWM_WHEEL, 0);

  //Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg&$");\

  loadEROM();

}

void loop() {
  checkOverRunMax();
  runSerialCommand();
  readStatus();
  calcWheelTargetA();
}




const int  iEROMLegIdAddress = 0;
const int  iEROMWheelMaxBackAddress = iEROMLegIdAddress + 2; 
const int  iEROMWheelMaxFrontAddress = iEROMWheelMaxBackAddress + 4; 
uint16_t  iEROMLegId = 0;
uint16_t  iEROMWheelMaxBack = 280; 
uint16_t  iEROMWheelMaxFront = 420; 

void loadEROM() {
  DUMP_VAR(EEPROM.length());
  {
    byte value1 = EEPROM.read(iEROMLegId);
    byte value2 = EEPROM.read(iEROMLegId+1);
    iEROMLegId = value1 | value2 << 8;
    DUMP_VAR(iEROMLegId);
  }
  {
    byte value1 = EEPROM.read(iEROMWheelMaxBackAddress);
    byte value2 = EEPROM.read(iEROMWheelMaxBackAddress+1);
    iEROMWheelMaxBack = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelMaxBack);
  }
  {
    byte value1 = EEPROM.read(iEROMWheelMaxFrontAddress);
    byte value2 = EEPROM.read(iEROMWheelMaxFrontAddress+1);
    iEROMWheelMaxFront = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelMaxFront);
  }
}
void saveEROM(int address,uint16_t value) {
  byte value1 =  value & 0xff;
  EEPROM.write(address,value1);
  byte value2 = (value >> 8) & 0xff;
  EEPROM.write(address+1,value2);
}


void A_Motor_FGS_By_Interrupt(void) {
}
void B_Motor_FGS_By_Interrupt(void) {
}


unsigned char speed_wheel = 0x0;
static long wheelRunCounter = -1;
static long const iRunTimeoutCounter = 10000L * 10L;
#define FRONT_A_WHEEL() { \
  digitalWrite(A_MOTER_CCW_WHEEL, LOW);\
}
#define BACK_A_WHEEL() { \
  digitalWrite(A_MOTER_CCW_WHEEL, HIGH);\
}
#define STOP_A_WHEEL() {\
  speed_wheel = 0x0;\
  analogWrite(A_MOTER_PWM_WHEEL, 0x0);\
}

int iCurrentLinear = 0;
int iVolumeDistanceWheel = 0;



int runMotorFGSignlCouter = 0;
int runMotorFGSignlCouter_NOT = 0;

void runWheel(int spd,int front,bool a) {
  speed_wheel = spd;
  if(a) {
    analogWrite(A_MOTER_PWM_WHEEL, spd);
  } else {
    analogWrite(B_MOTER_PWM_WHEEL, spd);
  }
  wheelRunCounter = iRunTimeoutCounter;
  runMotorFGSignlCouter = 0;
  runMotorFGSignlCouter_NOT = 0;
  if(front) {
    if(a) {
      digitalWrite(A_MOTER_CCW_WHEEL , HIGH);
    } else {
      digitalWrite(B_MOTER_CCW_WHEEL , HIGH);
    }
    //DUMP_VAR(front);
  } else {
    if(a) {
      digitalWrite(A_MOTER_CCW_WHEEL, LOW);
    } else {
      digitalWrite(B_MOTER_CCW_WHEEL , LOW);
    }
    //DUMP_VAR(front);
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
  res = "&$" + res;
  res += "&$";
  Serial.print(res);
}
void run_simple_command(void) {
  if(gSerialInputCommand=="uu") {
    speed_wheel -= 5;
    analogWrite(A_MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="dd") {
    speed_wheel += 5;
    analogWrite(A_MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="ff") {
    FRONT_A_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="bb") {
    BACK_A_WHEEL();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="ss") {
    speed_wheel =0xff;
    analogWrite(A_MOTER_CCW_WHEEL, speed_wheel);  
  }
  if(gSerialInputCommand=="gg") {
    speed_wheel =0;
    analogWrite(A_MOTER_CCW_WHEEL, speed_wheel);  
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
  resTex += ":mb,";
  resTex += String(iEROMWheelMaxBack);      
  resTex += ":mf,";
  resTex += String(iEROMWheelMaxFront);
  resTex += ":wp,";
  resTex += String(iVolumeDistanceWheel);
  resTex += ":lc,";
  resTex += String(iCurrentLinear);
  responseTextTag(resTex);
}

void runSetting(void) {
  int legID = 0;
  if(readTagValue(":id,","",&legID)) {
    //DUMP_VAR(legID);
    saveEROM(iEROMLegIdAddress,legID);
    iEROMLegId = legID;
  }
  int MaxFront = 0;
  if(readTagValue(":mf,","",&MaxFront)) {
    //DUMP_VAR(legID);
    saveEROM(iEROMWheelMaxFrontAddress,MaxFront);
    iEROMWheelMaxFront = MaxFront;
  }
  int MaxBack = 0;
  if(readTagValue(":mb,","",&MaxBack)) {
    //DUMP_VAR(legID);
    saveEROM(iEROMWheelMaxBackAddress,MaxBack);
    iEROMWheelMaxBack = MaxBack;
  }
}

void runWheel(void) {
  int volDist = 0;
  if(readTagValue(":v,",":vol,",&volDist)) {
    DUMP_VAR(volDist);
    runWheelVolume(volDist);
  }
}




void readStatus() {
  readLinearCurrent();
  readWheelVolume();
}

bool bIsRunWheelByVolume = false;


void checkOverRunMax(void) {
  // stop
  if(wheelRunCounter-- <= 0 ) {
    STOP_WHEEL();
  }
  if(iVolumeDistanceWheel < iEROMWheelMaxBack) {
    bIsRunWheelByVolume = false;
    STOP_WHEEL();
  }
  if(iVolumeDistanceWheel > iEROMWheelMaxFront) {
    bIsRunWheelByVolume = false;
    STOP_WHEEL();
  }  
}




int const iTargetDistanceMaxDiff = 1;




const int iConstVolumeDistanceWheelReportDiff = 1;
int iVolumeDistanceWheelReported = 0;

void readWheelVolume() {
  int volume = analogRead(MOTER_VOLUME_WHEEL);
  bool iReport = abs(volume - iVolumeDistanceWheelReported) > iConstVolumeDistanceWheelReportDiff;
  //DUMP_VAR(volume);
  //DUMP_VAR(abs(volume - iVolumeDistanceWheelReported));
  //bool iReport = true;
  if(iReport) {
    iVolumeDistanceWheelReported = volume;
    String resTex;
    resTex += "wheel:vol,";
    resTex += String(volume);
    responseTextTag(resTex);
  }
  iVolumeDistanceWheel = volume;
}

void readLinearCurrent() {
  int current = analogRead(MOTER_CURRENT_LINEAR);
  if(abs(current - 506) > 10) {
    //DUMP_VAR(current);
  }
  iCurrentLinear = current;
}

const int iConstStarSpeed = 254;

int iTargetVolumePostionWheel = 0;
void runWheelVolume(int distPostion) {
  if(distPostion < iEROMWheelMaxBack || distPostion > iEROMWheelMaxFront) {
    //DUMP_VAR(distPostion);
    //DUMP_VAR(iEROMWheelMaxBack);
    //DUMP_VAR(iEROMWheelMaxFront);
    return;
  }
  iTargetVolumePostionWheel = distPostion;
  bIsRunWheelByVolume = true;
  
  int moveDiff = iTargetVolumePostionWheel - iVolumeDistanceWheel;
  bool bForwardRunWheel = true;
  if(moveDiff > 0) {
    bForwardRunWheel = false;
  }
  //DUMP_VAR(bForwardRunWheel);
  if(bForwardRunWheel) {
    runWheel(iConstStarSpeed,1);
  } else {
    runWheel(iConstStarSpeed,0);
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
/*
int const aVolumeSpeedTable[] = {
  0,  0,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  iConstStarSpeed
};
*/

int const aVolumeSpeedTable[] = {
  0,  0,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  130,130,130,130,130,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  160,160,160,160,160,
  iConstStarSpeed
};


long const aVolumeSpeedTableLength = sizeof(aVolumeSpeedTable)/sizeof(aVolumeSpeedTable[0]);

int const iConstVolumeWheelNearTarget = 3;

void calcWheelTargetA() {
  if(bIsRunWheelByVolume == false) {
    return;
  }
  int moveDiff = iTargetVolumePostionWheel - iVolumeDistanceWheel;
  int distanceToMove = abs(moveDiff);
  if(distanceToMove < iConstVolumeWheelNearTarget) {
    bIsRunWheelByVolume = false;
    STOP_A_WHEEL();
    return;
  } /*else {
      MyJsonDoc doc;
      JsonObject root = doc.to<JsonObject>();
      root["moveDiff"] = moveDiff;
      root["bIsRunWheelByVolume"] = bIsRunWheelByVolume;
      repsponseJson(doc);
  }*/
  
  bool bForwardRunWheel = true;
  if(moveDiff > 0) {
    bForwardRunWheel = false;
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
  if(distPostion < iEROMWheelMaxBack || distPostion > iEROMWheelMaxFront) {
    DUMP_VAR(distPostion);
    DUMP_VAR(iEROMWheelMaxBack);
    DUMP_VAR(iEROMWheelMaxFront);
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
    runWheel(iConstStarSpeed,1);
  } else {
    runWheel(iConstStarSpeed,0);
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
  iConstStarSpeed
};
long const aFGSSpeedTableLength = sizeof(aFGSSpeedTable)/sizeof(aFGSSpeedTable[0]);




bool readTagValue(String tag,String shortTag , int *val) {
  int firstTag = gSerialInputCommand.indexOf(tag);
  if(firstTag > 0) {
    String tagStr = gSerialInputCommand.substring(firstTag+tag.length());
    DUMP_VAR(tagStr);
    int tagNum = tagStr.toInt();
    DUMP_VAR(tagNum);
    *val =tagNum;
    return true;
  }
  int firstShortTag = gSerialInputCommand.indexOf(shortTag);
  if(firstShortTag > 0) {
    String tagStr = gSerialInputCommand.substring(firstShortTag+shortTag.length());
    DUMP_VAR(tagStr);
    int tagNum = tagStr.toInt();
    DUMP_VAR(tagNum);
    *val =tagNum;
    return true;
  }
  return false;
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
      resTex += ",";
      resTex += String(valRes);      
      responseTextTag(resTex);
    } else {
      analogWrite(port,val);        
    }
  }
}
