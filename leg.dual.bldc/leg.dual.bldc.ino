#include <EEPROM.h>

uint8_t iEROMPWMLogLevel = 0; 

#define DUMP_VAR(x)  { \
  if(iEROMPWMLogLevel > 0 ) { \
    Serial.print(__LINE__);\
    Serial.print("@@"#x"=<");\
    Serial.print(x);\
    Serial.print(">&$");\
    Serial.print("\r\n");\
  }\
}



#define LOG_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">&$");\
  Serial.print("\r\n");\
}


#define GRAPH_VAR(x,y)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">");\
  Serial.print("@@"#y"=<");\
  Serial.print(y);\
  Serial.print(">&$");\
  Serial.print("\r\n");\
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
  //TCCR1B |= B00000011;

  pin_motor_setup(0);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL[0]),A_Motor_FGS_By_Interrupt , FALLING);

  pin_motor_setup(1);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL[1]),B_Motor_FGS_By_Interrupt , FALLING);

  Serial.begin(115200);
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
  readStatus();
  checkOverRunMax();
  calcWheelTarget(0);
  calcWheelTarget(1);
  runSerialCommand();
}




const int  iEROMLegIdAddress[MAX_MOTOR_CH] = {0,2};
const int  iEROMWheelMaxBackAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 2,iEROMLegIdAddress[1] + 4}; 
const int  iEROMWheelMaxFrontAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 6,iEROMLegIdAddress[1] + 8}; 
const int  iEROMCWDirectAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 10,iEROMLegIdAddress[1] + 12}; 
const int  iEROMPWMOffsetAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 14,iEROMLegIdAddress[1] + 16};
const int  iEROMZeroPositionAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 18,iEROMLegIdAddress[1] + 20};
const int  iEROMPayloadPWMOffsetAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 22,iEROMLegIdAddress[1] + 24};
const int  iEROMStartDelayAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 26,iEROMLegIdAddress[1] + 28};
const int  iEROMPWMFactorAddress[MAX_MOTOR_CH] = {iEROMLegIdAddress[1] + 30,iEROMLegIdAddress[1] + 32};
const int  iEROMPWMLogLevelAddress = iEROMLegIdAddress[1] + 256;


uint16_t  iEROMLegId[MAX_MOTOR_CH] = {0,0};
uint16_t  iEROMWheelMaxBack[MAX_MOTOR_CH] = {280,280}; 
uint16_t  iEROMWheelMaxFront[MAX_MOTOR_CH] = {420,420}; 
uint16_t  iEROMCWDirect[MAX_MOTOR_CH] = {1,0}; 
uint16_t  iEROMZeroPosition[MAX_MOTOR_CH] = {0,0};
uint16_t  iEROMStartDelay[MAX_MOTOR_CH] = {0,0};

int16_t  iEROMPWMOffset[MAX_MOTOR_CH] = {0,0};
int16_t  iEROMPayloadPWMOffset[MAX_MOTOR_CH] = {0,0};

uint16_t  iEROMPWMFactor[MAX_MOTOR_CH] = {4*100,4*100};

bool bZeroPositionNearSmall[MAX_MOTOR_CH] = {false,false};



void loadEROM1Byte(int address,uint8_t *dst) {
  uint8_t value1 = EEPROM.read(address);
  *dst = value1;
}

void loadEROM2Byte(int index,int address[],uint16_t dst[]) {
  uint16_t value1 = EEPROM.read(address[index]);
  uint16_t value2 = EEPROM.read(address[index]+1);
  dst[index] = value1 | value2 << 8;;
}

void loadEROM2ByteSign(int index,int address[],int16_t dst[]) {
  int16_t value1 = EEPROM.read(address[index]);
  int16_t value2 = EEPROM.read(address[index]+1);
  dst[index] = value1 | value2 << 8;;
}

void loadEROMFloat(int index,int address[],float dst[]) {
  uint32_t value1 = EEPROM.read(address[index]);
  uint32_t value2 = EEPROM.read(address[index]+1);
  uint32_t value3 = EEPROM.read(address[index]+2);
  uint32_t value4 = EEPROM.read(address[index]+3);
  uint32_t saveValue = value1 | value2 << 8 | value2 << 16 | value2 << 24;
  float value = saveValue;
  dst[index] = value;
}


void loadEROM(void) {
  loadEROM1Byte(iEROMPWMLogLevelAddress,&iEROMPWMLogLevel);
  
  loadEROM2Byte(0,iEROMLegIdAddress,iEROMLegId);
  loadEROM2Byte(1,iEROMLegIdAddress,iEROMLegId);
  
  loadEROM2Byte(0,iEROMWheelMaxBackAddress,iEROMWheelMaxBack);
  loadEROM2Byte(1,iEROMWheelMaxBackAddress,iEROMWheelMaxBack);
  loadEROM2Byte(0,iEROMWheelMaxFrontAddress,iEROMWheelMaxFront);
  loadEROM2Byte(1,iEROMWheelMaxFrontAddress,iEROMWheelMaxFront);
  
  loadEROM2Byte(0,iEROMCWDirectAddress,iEROMCWDirect);
  loadEROM2Byte(1,iEROMCWDirectAddress,iEROMCWDirect);
 
  
  loadEROM2ByteSign(0,iEROMPWMOffsetAddress,iEROMPWMOffset);
  loadEROM2ByteSign(1,iEROMPWMOffsetAddress,iEROMPWMOffset);

  loadEROM2Byte(0,iEROMZeroPositionAddress,iEROMZeroPosition);
  loadEROM2Byte(1,iEROMZeroPositionAddress,iEROMZeroPosition);

  loadEROM2ByteSign(0,iEROMPayloadPWMOffsetAddress,iEROMPayloadPWMOffset);
  loadEROM2ByteSign(1,iEROMPayloadPWMOffsetAddress,iEROMPayloadPWMOffset);

  loadEROM2Byte(0,iEROMStartDelayAddress,iEROMStartDelay);
  loadEROM2Byte(1,iEROMStartDelayAddress,iEROMStartDelay);

  loadEROM2Byte(0,iEROMPWMFactorAddress,iEROMPWMFactor);
  loadEROM2Byte(1,iEROMPWMFactorAddress,iEROMPWMFactor);

  //DUMP_VAR(iEROMWheelMaxFront[0]);
  //DUMP_VAR(iEROMZeroPosition[0]);
  //DUMP_VAR(iEROMWheelMaxBack[0]);
  
  //int16_t mfDist0 = (int16_t)iEROMWheelMaxFront[0] - (int16_t)iEROMZeroPosition[0];
  //int16_t mbDist0 = (int16_t)iEROMWheelMaxBack[0] - (int16_t)iEROMZeroPosition[0];
  //DUMP_VAR(mfDist0);
  //DUMP_VAR(mbDist0);
  bZeroPositionNearSmall[0] = (2*iEROMZeroPosition[0] <= iEROMWheelMaxFront[0] + iEROMWheelMaxBack[0]);


  //DUMP_VAR(iEROMWheelMaxFront[0]);
  //DUMP_VAR(iEROMZeroPosition[0]);
  //DUMP_VAR(iEROMWheelMaxBack[0]);


  //int16_t mfDist1 = (int16_t)iEROMWheelMaxFront[1]- (int16_t)iEROMZeroPosition[1];
  //int16_t mbDist1 = (int16_t)iEROMWheelMaxBack[1] - (int16_t)iEROMZeroPosition[1];
  //DUMP_VAR(mfDist1);
  //DUMP_VAR(mbDist1);
  bZeroPositionNearSmall[1] = (2*iEROMZeroPosition[1] <= iEROMWheelMaxFront[1] + iEROMWheelMaxBack[1]);

}


void saveEROM(int address,uint16_t value) {
  byte value1 =  value & 0xff;
  EEPROM.write(address,value1);
  byte value2 = (value >> 8) & 0xff;
  EEPROM.write(address+1,value2);
}

void saveEROM1Byte(int address,uint8_t *valueRam,String tag) {
  int valueTag = 0;
  if(readTagValue(tag,"",&valueTag)) {
    saveEROM(address,valueTag);
    *valueRam =  valueTag;
  }
}

void saveEROM2Byte(int index,int address[],uint16_t valueRam[],String tag) {
  int valueTag = 0;
  if(readTagValue(tag,"",&valueTag)) {
    saveEROM(address[index],valueTag);
    valueRam[index] =  valueTag;
  }
}



void saveEROM2ByteSign(int index,int address[],int16_t valueRam[],String tag) {
  int16_t valueTag = 0;
  if(readTagValue(tag,"",&valueTag)) {
    uint16_t save = valueTag;
    saveEROM(address[index],save);
    valueRam[index] =  valueTag;
  }
}

void runSetting(void) {
  saveEROM1Byte(iEROMPWMLogLevelAddress,&iEROMPWMLogLevel,":debug,");
  saveEROM1Byte(iEROMPWMLogLevelAddress,&iEROMPWMLogLevel,":log,");

  saveEROM2Byte(0,iEROMLegIdAddress,iEROMLegId,":id0,");
  saveEROM2Byte(1,iEROMLegIdAddress,iEROMLegId,":id1,");

  saveEROM2Byte(0,iEROMWheelMaxFrontAddress,iEROMWheelMaxFront,":mf0,");
  saveEROM2Byte(1,iEROMWheelMaxFrontAddress,iEROMWheelMaxFront,":mf1,");
  saveEROM2Byte(0,iEROMWheelMaxBackAddress,iEROMWheelMaxBack,":mb0,");
  saveEROM2Byte(1,iEROMWheelMaxBackAddress,iEROMWheelMaxBack,":mb1,");


  saveEROM2Byte(0,iEROMCWDirectAddress,iEROMCWDirect,":cw0,");
  saveEROM2Byte(1,iEROMCWDirectAddress,iEROMCWDirect,":cw1,");
  
  saveEROM2ByteSign(0,iEROMPWMOffsetAddress,iEROMPWMOffset,":pwm0,");
  saveEROM2ByteSign(1,iEROMPWMOffsetAddress,iEROMPWMOffset,":pwm1,");

  saveEROM2Byte(0,iEROMZeroPositionAddress,iEROMZeroPosition,":zeroP0,");
  saveEROM2Byte(1,iEROMZeroPositionAddress,iEROMZeroPosition,":zeroP1,");

  saveEROM2ByteSign(0,iEROMPayloadPWMOffsetAddress,iEROMPayloadPWMOffset,":payloadpwm0,");
  saveEROM2ByteSign(1,iEROMPayloadPWMOffsetAddress,iEROMPayloadPWMOffset,":payloadpwm1,");

  saveEROM2Byte(0,iEROMStartDelayAddress,iEROMStartDelay,":startdelay0,");
  saveEROM2Byte(1,iEROMStartDelayAddress,iEROMStartDelay,":startdelay1,");


  saveEROM2Byte(0,iEROMPWMFactorAddress,iEROMPWMFactor,":pwmGain0,");
  saveEROM2Byte(1,iEROMPWMFactorAddress,iEROMPWMFactor,":pwmGain1,");

}



void A_Motor_FGS_By_Interrupt(void) {
}
void B_Motor_FGS_By_Interrupt(void) {
}


unsigned char speed_wheel[MAX_MOTOR_CH] = {0,0};
static long wheelRunCounter[MAX_MOTOR_CH] = {-1,-1};
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

void runWheel(int spd,bool front,int index) {
  speed_wheel[index] = spd;
  if(front) {
    digitalWrite(MOTER_CCW_WHEEL[index] , HIGH);
    //DUMP_VAR(front);
  } else {
    digitalWrite(MOTER_CCW_WHEEL[index], LOW);
    //DUMP_VAR(front);
  }
  analogWrite(MOTER_PWM_WHEEL[index], spd);
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
  res += "&$\r\n";
  Serial.print(res);
}

void responseTextContinue(String &res) {
  res = "&$" + res;
  Serial.print(res);
}

void responseTextFinnish(String &res) {
  res = res;
  res += "&$\r\n";
  Serial.print(res);
}


void run_simple_command(void) {
  if(gSerialInputCommand=="uu") {
    speed_wheel[0] -= 5;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel[0]);  
  }
  if(gSerialInputCommand=="dd") {
    speed_wheel[0] += 5;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel[0]);  
  }
  if(gSerialInputCommand=="ff") {
    FRONT_WHEEL(0);
    wheelRunCounter[0] = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="bb") {
    BACK_WHEEL(0);
    wheelRunCounter[0] = iRunTimeoutCounter;
  }
  if(gSerialInputCommand=="ss") {
    speed_wheel[0] =0xff;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel[0]);  
  }
  if(gSerialInputCommand=="gg") {
    speed_wheel[0] =0;
    analogWrite(MOTER_CCW_WHEEL[0], speed_wheel[0]);  
  }
}


void run_comand(void) {
  //DUMP_VAR(InputCommand);
  //DUMP_VAR(speed_wheel[0]);
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
    runWheelByTag();
  }
  if(gSerialInputCommand.startsWith("who:") || gSerialInputCommand.startsWith("H:")) {
    whois();
  }
  if(gSerialInputCommand.startsWith("legM:") || gSerialInputCommand.startsWith("M:")) {
    moveLegToPosition();
  }
  if(gSerialInputCommand.startsWith("legG:") || gSerialInputCommand.startsWith("P:")) {
    getLegPosition();
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
  resTex += ":cw0,";
  resTex += String(iEROMCWDirect[0]);
  resTex += ":pw0,";
  resTex += String(iEROMPWMOffset[0]);
  resTex += ":zp0,";
  resTex += String(iEROMZeroPosition[0]);
  resTex += ":ns0,";
  resTex += String(bZeroPositionNearSmall[0]);
  resTex += ":pl0,";
  resTex += String(iEROMPayloadPWMOffset[0]);
  resTex += ":sd0,";
  resTex += String(iEROMStartDelay[0]);
  resTex += ":pwmGain0,";
  resTex += String(iEROMPWMFactor[0]);
  responseTextContinue(resTex);
  resTex = "";
  resTex += ":mb1,";
  resTex += String(iEROMWheelMaxBack[1]);      
  resTex += ":mf1,";
  resTex += String(iEROMWheelMaxFront[1]);
  resTex += ":wp1,";
  resTex += String(iVolumeDistanceWheel[1]);
  resTex += ":cw1,";
  resTex += String(iEROMCWDirect[1]);
  resTex += ":pw1,";
  resTex += String(iEROMPWMOffset[1]);
  resTex += ":zp1,";
  resTex += String(iEROMZeroPosition[1]);
  resTex += ":ns1,";
  resTex += String(bZeroPositionNearSmall[1]);
  resTex += ":pl1,";
  resTex += String(iEROMPayloadPWMOffset[1]);
  resTex += ":sd1,";
  resTex += String(iEROMStartDelay[1]);
  resTex += ":lv,";
  resTex += String(iEROMPWMLogLevel);
  resTex += ":pwmGain1,";
  resTex += String(iEROMPWMFactor[1]);
  responseTextFinnish(resTex);
}

void whois(void) {
  String resTex;
  resTex += "arduino";
  responseTextTag(resTex);
}



void runWheelByTag(void) {
  int volDistA = 0;
  if(readTagValue(":v0,",":vol0,",&volDistA)) {
    DUMP_VAR(volDistA);
    runWheelVolume(volDistA,0,0);
  }
  int volDistB = 0;
  if(readTagValue(":v1,",":vol1,",&volDistB)) {
    DUMP_VAR(volDistB);
    runWheelVolume(volDistB,1,0);
  }
}


void readStatus() {
  readWheelVolume(0);
  readWheelVolume(1);
}

bool bIsRunWheelByVolume[MAX_MOTOR_CH] = {false,false};

void checkOverRunMaxWheel(int index) {
  if(iVolumeDistanceWheel[index] > iEROMWheelMaxBack[index]) {
    bIsRunWheelByVolume[index] = false;
    STOP_WHEEL(index);
  }
  if(iVolumeDistanceWheel[index] < iEROMWheelMaxFront[index]) {
    bIsRunWheelByVolume[index] = false;
    STOP_WHEEL(index);
  }  
}
void checkOverRunMax(void) {

  // stop
  if(wheelRunCounter[0]-- <= 0 ) {
    STOP_WHEEL(0);
  }  
  if(wheelRunCounter[1]-- <= 0 ) {
    STOP_WHEEL(1);
  }  
  checkOverRunMaxWheel(0);
  checkOverRunMaxWheel(1);
}

void moveLegToPosition() {
  int idLeg = 0;
  if(readTagValue(":id,",":id,",&idLeg)) {
    DUMP_VAR(idLeg);
    int legIndex = -1;
    if(iEROMLegId[0] == idLeg) {
      legIndex = 0;
    }
    if(iEROMLegId[1] == idLeg) {
      legIndex = 1;
    }
    if(legIndex < 0 ){
      String resTex;
      resTex += "legM:0";
      resTex += ",legIndex:";
      resTex += String(legIndex);
      responseTextTag(resTex);
      return ;
    }
    DUMP_VAR(legIndex);
    int payload = 0;
    if(readTagValue(":payload,",":payload,",&payload)) {
    }
    int position = -1;
    if(readTagValue(":xmm,",":xmm,",&position)) {
      
      DUMP_VAR(position);
      int volDist = calcVolumeFromMM(legIndex,position);
      runWheelVolume(volDist,legIndex,payload);
      String resTex;
      resTex += "legM:1";
      resTex += ",volDist:";
      resTex += String(volDist);
      resTex += ",legIndex:";
      resTex += String(legIndex);
      responseTextTag(resTex);
      return;
    }
  }
  String resTex;
  resTex += "legM:0";
  responseTextTag(resTex);
}


const float fMM2VolumeFactor = 0.98;
int calcVolumeFromMM(int index,int mm) {
  int zeroP = iEROMZeroPosition[index];
  int moveInVolume = 0;
  if(bZeroPositionNearSmall[index]) {
    moveInVolume = fMM2VolumeFactor * (float)mm;
  } else {
    moveInVolume = 0 - fMM2VolumeFactor * (float)mm;
  }
  int volume =  moveInVolume + zeroP;
  return volume;
}

void getLegPosition() {
}

int const iTargetDistanceMaxDiff = 1;




const int iConstVolumeDistanceWheelReportDiff = 2;
const int iConstVolumeDistanceWheelReportDiffBigRange = 10;

int iVolumeDistanceWheelReported[MAX_MOTOR_CH] = {0,0};

const String strConstWheelReportTag[MAX_MOTOR_CH] = {"wheel:vol0,","wheel:vol1,"};

int iDetectDistanceStartMemo[MAX_MOTOR_CH] = {0,0};
int iDetectIndex = -1;

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
  if(iDetectIndex > -1 && iDetectIndex == index) {
    int detectDiff = volume - iDetectDistanceStartMemo[iDetectIndex];
    if(abs(detectDiff) > 5) {
      DUMP_VAR(detectDiff);
      iDetectIndex = -1;
      if(detectDiff > 0) {
        saveEROM(iEROMCWDirectAddress[index],1);
      } else {
        saveEROM(iEROMCWDirectAddress[index],0);
      }
    }
  }
}


const uint16_t iConstStarSpeed = 254;

int iTargetVolumePostionWheel[MAX_MOTOR_CH] = {0,0};
int iTargetVolumePayload[MAX_MOTOR_CH] = {0,0};
int iTargetVolumeStartDelay[MAX_MOTOR_CH] = {0,0};
void runWheelVolume(int distPostion,int index,int payload) {
  {
    String resTex;
    resTex += "dummy:distPostion,";
    resTex += String(distPostion);
    resTex += ":index,";
    resTex += String(index);
    resTex += ":iEROMWheelMaxBack,";
    resTex += String(iEROMWheelMaxBack[index]);
    resTex += ":iEROMWheelMaxFront,";
    resTex += String(iEROMWheelMaxFront[index]);
    responseTextTag(resTex);
  }
  if(distPostion > iEROMWheelMaxBack[index] || distPostion < iEROMWheelMaxFront[index]) {
    //DUMP_VAR(distPostion);
    //DUMP_VAR(iEROMWheelMaxBack[index]);
    //DUMP_VAR(iEROMWheelMaxFront[index]);
    return;
  }
  iTargetVolumePostionWheel[index] = distPostion;
  bIsRunWheelByVolume[index] = true;
  wheelRunCounter[index] = iRunTimeoutCounter;

  
  int moveDiff = iTargetVolumePostionWheel[index] - iVolumeDistanceWheel[index];
  bool bForwardRunWheel;
  if(moveDiff > 0) {
    bForwardRunWheel = iEROMCWDirect[index];;
  } else {
    bForwardRunWheel = !iEROMCWDirect[index];;
  }
  DUMP_VAR(bForwardRunWheel);
  iTargetVolumeStartDelay[index] = iEROMStartDelay[index];
  if(iTargetVolumeStartDelay[index] < 1) {
    runWheel(iConstStarSpeed,bForwardRunWheel,index);
  }
  iTargetVolumePayload[index] = payload;

  {
    String resTex;
    resTex += "dummy:bIsRunWheelByVolume,";
    resTex += String(bIsRunWheelByVolume[index]);
    resTex += ":iTargetVolumePostionWheel,";
    resTex += String(iTargetVolumePostionWheel[index]);
    resTex += ":iTargetVolumePayload,";
    resTex += String(iTargetVolumePayload[index]);
    responseTextTag(resTex);
  }

}



/*
int const aVolumeSpeedTable[] = {
  60,60,60,60,60,
  70,70,70,70,70,
  80,80,80,80,80,
  90,90,90,90,90,
  100,100,100,100,100,
  160,160,160,160,160,
  iConstStarSpeed
};
*/


int const aVolumeSpeedTable[] = {
  60,60,60,60,60,
  70,70,70,70,70,
  80,80,80,80,80,
  90,90,90,90,90,
  100,100,100,100,100,
  100,100,100,100,100,
  100,100,100,100,100,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
  127,127,127,127,127,
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


/*
int const aVolumeSpeedTable[] = {
  0,  0,130,130,130,
  130,130,130,130,130,
  160,160,160,160,160,
  160,160,160,160,160,
  iConstStarSpeed
};
*/

long const aVolumeSpeedTableLength = sizeof(aVolumeSpeedTable)/sizeof(aVolumeSpeedTable[0]);

int const iConstVolumeWheelNearTarget = 5;
//const float fConstSpeedK = 4;
uint32_t iConstPWMFactorDive = 100;
int calWheelSpeedPwm(int16_t eDistance,int index) {
  uint32_t distance = abs(eDistance);
  uint32_t pwmK = iEROMPWMFactor[index] * distance / iConstPWMFactorDive;
  if(pwmK > iConstStarSpeed) {
    return iConstStarSpeed;
  }
  int pwm = pwmK;
  return pwm;
}
bool calWheelCW(int16_t eDistance,int index) {
  bool bForwardRunWheel;
  if(eDistance > 0) {
    bForwardRunWheel = iEROMCWDirect[index];
  } else {
    bForwardRunWheel = !iEROMCWDirect[index];
  }
  return bForwardRunWheel;  
}

void calcWheelTarget(int index) {
  if(bIsRunWheelByVolume[index] == false) {
    return;
  }
  int16_t current = iVolumeDistanceWheel[index];
  int16_t target = iTargetVolumePostionWheel[index];
  int16_t toMoveDiff =  target - current;
  int speedPwm = calWheelSpeedPwm(toMoveDiff,index);
  bool bForwardRunWheel = calWheelCW(toMoveDiff,index);
  runWheel(speedPwm,bForwardRunWheel,index);
  
  {
    String resTex;
    resTex += "dummy:bForwardRunWheelffset,";
    resTex += String(bForwardRunWheel);
    resTex += ":speedPwm,";
    resTex += String(speedPwm);
    resTex += ":leg,";
    resTex += String(iEROMLegId[index]);
    responseTextTag(resTex);
  }
}
/*
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
  }
  if(iTargetVolumeStartDelay[index] > 0) {
    {
      String resTex;
      resTex += "dummy:delay,";
      resTex += String(iTargetVolumeStartDelay[index]);
      responseTextTag(resTex);
    }
    iTargetVolumeStartDelay[index]--;
    return;
  }

  
  bool bForwardRunWheel;
  if(moveDiff > 0) {
    bForwardRunWheel = iEROMCWDirect[index];
  } else {
    bForwardRunWheel = !iEROMCWDirect[index];
  }
  //DUMP_VAR(bForwardRunWheel);
  DUMP_VAR(distanceToMove);
  int speedIndex = distanceToMove;
  DUMP_VAR(speedIndex);
  if(distanceToMove >= aVolumeSpeedTableLength) {
    speedIndex = aVolumeSpeedTableLength -1;
  }
  uint16_t speed = aVolumeSpeedTable[speedIndex];
  DUMP_VAR(speed);
  if(iTargetVolumePayload[index] > 0) {
    speed += iEROMPayloadPWMOffset[index];
    #if 0
    {
      String resTex;
      resTex += "dummy:payloadpwmoffset,";
      resTex += String(iEROMPayloadPWMOffset[index]);
      resTex += ":leg,";
      resTex += String(iEROMLegId[index]);
      responseTextTag(resTex);
    }
    #endif
  } else {
    speed += iEROMPWMOffset[index];
    #if 0
    {
      String resTex;
      resTex += "dummy:pwmoffset,";
      resTex += String(iEROMPWMOffset[index]);
      resTex += ":leg,";
      resTex += String(iEROMLegId[index]);
      responseTextTag(resTex);
    }
    #endif
  }
  if(speed > iConstStarSpeed) {
    speed = iConstStarSpeed;
  }
  DUMP_VAR(speed);
  if(iTargetVolumeStartDelay[index] == 0) {
    speed = iConstStarSpeed;
  }
  runWheel(speed,bForwardRunWheel,index);
  GRAPH_VAR(speed,distanceToMove);
}
*/


bool readTagValue(String tag,String shortTag , int16_t *val) {
  int firstTag = gSerialInputCommand.indexOf(tag);
  if(firstTag > 0) {
    String tagStr = gSerialInputCommand.substring(firstTag+tag.length());
    DUMP_VAR(tagStr);
    int16_t tagNum = tagStr.toInt();
    DUMP_VAR(tagNum);
    *val =tagNum;
    return true;
  }
  int firstShortTag = gSerialInputCommand.indexOf(shortTag);
  if(firstShortTag > 0) {
    String tagStr = gSerialInputCommand.substring(firstShortTag+shortTag.length());
    DUMP_VAR(tagStr);
    int16_t tagNum = tagStr.toInt();
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
