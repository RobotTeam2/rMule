#include <EEPROM.h>

#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">&$");\
}

#define MAX_MOTOR_CH (2)

// Interrupt
const static char MOTER_FGS_WHEEL[MAX_MOTOR_CH] = {2,3};
const static char MOTER_PWM_WHEEL[MAX_MOTOR_CH] = {9,10};
const static char MOTER_CCW_WHEEL[MAX_MOTOR_CH] = {4,8};
const static char MOTER_VOLUME_WHEEL[MAX_MOTOR_CH] = {A1,A2};

void A_Motor_FGS_By_Interrupt(void);
void B_Motor_FGS_By_Interrupt(void);
void loadEROM(void);

void setup()
{
  // set pwm 9,10
  TCCR1B &= B11111000;
  TCCR1B |= B00000001;

  pin_motor_setup(0);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL[0]),A_Motor_FGS_By_Interrupt , FALLING);

  pin_motor_setup(1);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL[1]),B_Motor_FGS_By_Interrupt , FALLING);

  //Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg&$");\

  loadEROM();
}

void pin_motor_setup(int index) {
  pinMode(MOTER_CCW_WHEEL[index], OUTPUT);
  pinMode(MOTER_FGS_WHEEL[index], INPUT_PULLUP);
  pinMode(MOTER_PWM_WHEEL[index], OUTPUT);
  pinMode(MOTER_VOLUME_WHEEL[index], INPUT_PULLUP);
  analogWrite(MOTER_PWM_WHEEL[index], 0);
}


void checkOverRunMax(void);
void runSerialCommand(void);
void readStatus(void);
void calcWheelTarget(int index);
void loop() {
  checkOverRunMax();
  runSerialCommand();
  readStatus();
  calcWheelTarget(0);
  calcWheelTarget(1);
}




const int  iEROMLegIdAddress[MAX_MOTOR_CH] = {0,2};
const int  iEROMWheelMaxBackAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 2,iEROMLegIdAddress[1] + 4}; 
const int  iEROMWheelMaxFrontAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 6,iEROMLegIdAddress[1] + 8}; 

uint16_t  iEROMLegId[MAX_MOTOR_CH] = {0,0};
uint16_t  iEROMWheelMaxBack[MAX_MOTOR_CH] = {280,280}; 
uint16_t  iEROMWheelMaxFront[MAX_MOTOR_CH] = {420,420}; 


void loadEROMLimitSetting(int index) {
  {
    byte value1 = EEPROM.read(iEROMWheelMaxBackAddress[index]);
    byte value2 = EEPROM.read(iEROMWheelMaxBackAddress[index]+1);
    iEROMWheelMaxBack[index] = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelMaxBack[index]);
  }
  {
    byte value1 = EEPROM.read(iEROMWheelMaxFrontAddress[index]);
    byte value2 = EEPROM.read(iEROMWheelMaxFrontAddress[index]+1);
    iEROMWheelMaxFront[index] = value1 | value2 << 8;
    DUMP_VAR(iEROMWheelMaxFront[index]);
  }
}

void loadEROMLegID(int index) {
  byte value1 = EEPROM.read(iEROMLegIdAddress[index]);
  byte value2 = EEPROM.read(iEROMLegIdAddress[index]+1);
  iEROMLegId[index] = value1 | value2 << 8;
  DUMP_VAR(iEROMLegId[index]);
}

void loadEROM(void) {
  loadEROMLegID(0);
  loadEROMLegID(1);
  loadEROMLimitSetting(0);
  loadEROMLimitSetting(1);
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


unsigned char speed_wheel[MAX_MOTOR_CH] = {0,0};
static long wheelRunCounter = -1;
static long const iRunTimeoutCounter = 10000L * 10L;

#define FRONT_WHEEL(index) { \
  digitalWrite(MOTER_CCW_WHEEL[index], LOW);\
}
#define BACK_WHEEL(index) { \
  digitalWrite(MOTER_CCW_WHEEL[index], HIGH);\
}
#define STOP_WHEEL(index) {\
  speed_wheel[index] = 0x0;\
  analogWrite(MOTER_PWM_WHEEL[index], 0x0);\
}



int iVolumeDistanceWheel[2] = {};



int runMotorFGSignlCouter = 0;
int runMotorFGSignlCouter_NOT = 0;

void runWheel(int spd,int front,int index) {
  speed_wheel[index] = spd;
  analogWrite(MOTER_PWM_WHEEL[index], spd);
  wheelRunCounter = iRunTimeoutCounter;
  runMotorFGSignlCouter = 0;
  runMotorFGSignlCouter_NOT = 0;
  if(front) {
    digitalWrite(MOTER_CCW_WHEEL[index] , HIGH);
    //DUMP_VAR(front);
  } else {
    digitalWrite(MOTER_CCW_WHEEL[index], LOW);
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
    speed_wheel[0] -= 5;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel);  
  }
  if(gSerialInputCommand=="dd") {
    speed_wheel[0] += 5;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel);  
  }
  if(gSerialInputCommand=="ff") {
    FRONT_WHEEL(0);
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="bb") {
    BACK_WHEEL(0);
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="ss") {
    speed_wheel[0] =0xff;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel);  
  }
  if(gSerialInputCommand=="gg") {
    speed_wheel[0] =0;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel);  
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
  resTex += "info";
  resTex += ":ch,";
  resTex += String(MAX_MOTOR_CH);      
  resTex += ":id0,";
  resTex += String(iEROMLegId[0]);      
  resTex += ":id1,";
  resTex += String(iEROMLegId[1]);      
  resTex += ":mb0,";
  resTex += String(iEROMWheelMaxBack[0]);      
  resTex += ":mf0,";
  resTex += String(iEROMWheelMaxFront[0]);
  resTex += ":wp0,";
  resTex += String(iVolumeDistanceWheel[0]);
  resTex += ":mb1,";
  resTex += String(iEROMWheelMaxBack[1]);      
  resTex += ":mf1,";
  resTex += String(iEROMWheelMaxFront[1]);
  resTex += ":wp1,";
  resTex += String(iVolumeDistanceWheel[1]);
  responseTextTag(resTex);
}

void runLimmitSetting(int index) {
  int MaxFront = 0;
  String tagmf = ":mf0,";
  if(index > 0) {
    tagmf = ":mf1,";
  }
  if(readTagValue(tagmf,"",&MaxFront)) {
    //DUMP_VAR(MaxFront);
    saveEROM(iEROMWheelMaxFrontAddress[index],MaxFront);
    iEROMWheelMaxFront[index] = MaxFront;
  }
  int MaxBack = 0;
  String tagmb = ":mb0,";
  if(index > 0) {
    tagmb = ":mb1,";
  }
  if(readTagValue(tagmb,"",&MaxBack)) {
    //DUMP_VAR(MaxBackA);
    saveEROM(iEROMWheelMaxBackAddress[index],MaxBack);
    iEROMWheelMaxBack[index] = MaxBack;
  }
}
void runSetting(void) {
  int legIDA = 0;
  if(readTagValue(":id0,","",&legIDA)) {
    //DUMP_VAR(legIDA);
    saveEROM(iEROMLegIdAddress[0],legIDA);
    iEROMLegId[0] =  legIDA;
  }
  int legIDB = 0;
  if(readTagValue(":id1,","",&legIDB)) {
    //DUMP_VAR(legIDB);
    saveEROM(iEROMLegIdAddress[1],legIDB);
    iEROMLegId[1] =  legIDB;
  }

  runLimmitSetting(0);
  runLimmitSetting(1);
}

void runWheel(void) {
  int volDistA = 0;
  if(readTagValue(":v0,",":vol0,",&volDistA)) {
    DUMP_VAR(volDistA);
    runWheelVolume(volDistA,0);
  }
  int volDistB = 0;
  if(readTagValue(":v1,",":vol1,",&volDistB)) {
    DUMP_VAR(volDistB);
    runWheelVolume(volDistB,1);
  }
}




void readStatus() {
  readWheelVolume(0);
  readWheelVolume(1);
}

bool bIsRunWheelByVolume[MAX_MOTOR_CH] = {false,false};

void checkOverRunMaxWheel(int index) {
  if(iVolumeDistanceWheel[index] < iEROMWheelMaxBack[index]) {
    bIsRunWheelByVolume[index] = false;
    STOP_WHEEL(index);
  }
  if(iVolumeDistanceWheel[index] > iEROMWheelMaxFront[index]) {
    bIsRunWheelByVolume[index] = false;
    STOP_WHEEL(index);
  }  
}
void checkOverRunMax(void) {
  // stop
  if(wheelRunCounter-- <= 0 ) {
    STOP_WHEEL(0);
    STOP_WHEEL(1);
  }
  checkOverRunMaxWheel(0);
  checkOverRunMaxWheel(1);
}




int const iTargetDistanceMaxDiff = 1;




const int iConstVolumeDistanceWheelReportDiff = 3;
const int iConstVolumeDistanceWheelReportDiffBigRange = 10;

int iVolumeDistanceWheelReported[MAX_MOTOR_CH] = {0,0};

const String strConstWheelReportTag[MAX_MOTOR_CH] = {"wheel:vol0,","wheel:vol1,"};

void readWheelVolume(int index) {
  int volume = analogRead(MOTER_VOLUME_WHEEL[index]);  
  bool iReport = abs(volume - iVolumeDistanceWheelReported[index]) > iConstVolumeDistanceWheelReportDiff;
  if(volume > 1000) {
    iReport = abs(volume - iVolumeDistanceWheelReported[index]) > iConstVolumeDistanceWheelReportDiffBigRange;
  }
  //DUMP_VAR(abs(volume - iVolumeDistanceWheelReported[index]));
  //DUMP_VAR(volume);
  //bool iReport = true;
  if(iReport) {
    iVolumeDistanceWheelReported[index] = volume;
    String resTex;
    resTex += strConstWheelReportTag[index];
    resTex += String(volume);
    responseTextTag(resTex);
  }
  iVolumeDistanceWheel[index] = volume;
}


const int iConstStarSpeed = 254;

int iTargetVolumePostionWheel[MAX_MOTOR_CH] = {0,0};
void runWheelVolume(int distPostion,int index) {
  if(distPostion < iEROMWheelMaxBack[index] || distPostion > iEROMWheelMaxFront[index]) {
    //DUMP_VAR(distPostion);
    //DUMP_VAR(iEROMWheelMaxBack[index]);
    //DUMP_VAR(iEROMWheelMaxFront[index]);
    return;
  }
  iTargetVolumePostionWheel[index] = distPostion;
  bIsRunWheelByVolume[index] = true;
  
  int moveDiff = iTargetVolumePostionWheel[index] - iVolumeDistanceWheel[index];
  bool bForwardRunWheel = true;
  if(moveDiff > 0) {
    bForwardRunWheel = false;
  }
  //DUMP_VAR(bForwardRunWheel);
  if(bForwardRunWheel) {
    runWheel(iConstStarSpeed,1,index);
  } else {
    runWheel(iConstStarSpeed,0,index);
  }
}



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

void calcWheelTarget(int index) {
  if(bIsRunWheelByVolume[index] == false) {
    return;
  }
  int moveDiff = iTargetVolumePostionWheel[index] - iVolumeDistanceWheel[index];
  int distanceToMove = abs(moveDiff);
  if(distanceToMove < iConstVolumeWheelNearTarget) {
    bIsRunWheelByVolume[index] = false;
    STOP_WHEEL(index);
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
    runWheel(speed,1,index);
  } else {
    runWheel(speed,0,index);
  }
}



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