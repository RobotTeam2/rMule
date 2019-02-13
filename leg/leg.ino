#include <ArduinoJson.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <EEPROM.h>

VL53L0X sensor;
#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  Serial.print(x);\
  Serial.print(">\n");\
}


const static char MOTER_PWM_WHEEL = 12;
const static char MOTER_CCW_WHEEL = 27;

// Interrupt
const static char MOTER_FGS_WHEEL = 2;



const static char MOTER_CURRENT_LINEAR = 2;
const static char MOTER_PWM_LINEAR = 3;
const static char MOTER_STANDBY_LINEAR = 5;
const static char MOTER_A1_LINEAR = 7;
const static char MOTER_A2_LINEAR = 9;



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
uint16_t  iEROMWheelLimitBack = 120; 
uint16_t  iEROMWheelLimitFront = 500; 

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

#define TOF_HIGH_SPEED
//#define TOF_HIGH_ACCURACY

const int TOF_TIME_OUT = 1;

void setupTof() {
  Wire.begin();
  sensor.init();
  sensor.setTimeout(TOF_TIME_OUT);
  sensor.setSignalRateLimit(0.5);
  int limit_Mcps = sensor.getSignalRateLimit();
  DUMP_VAR(limit_Mcps);
/*
#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif
*/

#if defined TOF_HIGH_SPEED
  // reduce timing budget to 1 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(1000);
#elif defined TOF_HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif
}

void repsponseJson(const StaticJsonDocument<256> &doc) {
  String output;
  serializeJson(doc, output);
  Serial.print(output);
  Serial.print("\n");
}

int iDistanceWheelTof = 0;
const int iDistanceDiffMaxWheel = 40;

int iDistanceTofReportCounter = 1;
const int iDistanceTofReportSkip = 50;

void readTof() {
  int distance = sensor.readRangeSingleMillimeters();
  //DUMP_VAR(distance);
  if(distance <= 0 || distance >= 8191) {
    return ;
  }
  bool isReport = (iDistanceTofReportCounter++%iDistanceTofReportSkip) == 0;
  if(isReport) {
    StaticJsonDocument<256> doc;
    JsonObject root = doc.to<JsonObject>();
    root["tofw"] = distance;
    root["tofw_p"] = iDistanceWheelTof;
    repsponseJson(doc);
  }
  iDistanceWheelTof = distance;
}





void setup()
{
  pinMode(MOTER_PWM_WHEEL, OUTPUT);
  pinMode(MOTER_CCW_WHEEL, OUTPUT);

  //pinMode(MOTER_FGS_WHEEL, INPUT);
  //pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTER_FGS_WHEEL),CounterWheelFGSByInterrupt , FALLING);
  
  

  pinMode(MOTER_CURRENT_LINEAR, INPUT);
  pinMode(MOTER_PWM_LINEAR, OUTPUT);
  pinMode(MOTER_STANDBY_LINEAR, OUTPUT);
  pinMode(MOTER_A1_LINEAR, OUTPUT);
  pinMode(MOTER_A2_LINEAR, OUTPUT);
  

  digitalWrite(MOTER_PWM_LINEAR, HIGH);
  digitalWrite(MOTER_STANDBY_LINEAR, HIGH);
  digitalWrite(MOTER_A1_LINEAR, HIGH);
  digitalWrite(MOTER_A2_LINEAR, LOW);


  TCCR1B = (TCCR1B & 0b11111000) | 0x01;
  
  STOP_WHEEL();

//  Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg\n");\

  setupTof();
}

String InputCommand ="";




int runMotorFGSignlCouter = 0;
int runMotorFGSignlCouter_NOT = 0;

void CounterWheelFGSByInterrupt(void) {
  DUMP_VAR(++runMotorFGSignlCouter);
}

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

int iTargetDistanceWheel = 0;
int iStartDistanceWheel = 0;
int iTotalDistanceWheel = 0;
bool bIsRunWheelByTof = false;
void runWheelTof(int distPostion) {
  if(distPostion < iEROMWheelLimitBack || distPostion > iEROMWheelLimitFront) {
    DUMP_VAR(distPostion);
    DUMP_VAR(iEROMWheelLimitBack);
    DUMP_VAR(iEROMWheelLimitFront);
    return;
  }
  iTargetDistanceWheel = distPostion;
  iStartDistanceWheel = iDistanceWheelTof;
  iTotalDistanceWheel = abs(iTargetDistanceWheel - iStartDistanceWheel);
  DUMP_VAR(iTargetDistanceWheel);
  DUMP_VAR(iStartDistanceWheel);
  DUMP_VAR(iTotalDistanceWheel);
  bIsRunWheelByTof = true;
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
void runLinearTof(int distPostion) {
}


void tryConfirmJson() {
  StaticJsonDocument<256> jsonBuffer;
  auto errorDS = deserializeJson(jsonBuffer,InputCommand);
  if (errorDS) {
    DUMP_VAR(errorDS.c_str());
    return;
  }
  InputCommand = "";
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
    if(command == "tof" || command == "T") {
      int tofWheelDistance = -1;
      if(params.containsKey("wheel")) {
        tofWheelDistance = params["wheel"];
      }
      if(params.containsKey("W")) {
        tofWheelDistance = params["W"];
      }
      if(tofWheelDistance > 0) {
        runWheelTof(tofWheelDistance);
      }
      
      int tofLinearDistance = -1;
      if(params.containsKey("linear")) {
        tofLinearDistance = params["linear"];
      }
      if(params.containsKey("L")) {
        tofLinearDistance = params["L"];
      }
      if(tofLinearDistance > 0) {
        runLinearTof(tofLinearDistance);
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
      StaticJsonDocument<256> docRes;
      JsonObject rootRes = docRes.to<JsonObject>();
      rootRes["leg"] = iEROMLegId;
      rootRes["iEROMWheelLimitBack"] = iEROMWheelLimitBack;
      rootRes["iEROMWheelLimitFront"] = iEROMWheelLimitFront;
      repsponseJson(docRes);
    }
  }
}

void run_comand() {
  DUMP_VAR(InputCommand);
  DUMP_VAR(speed_wheel);
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
}


void readStatus() {
  int current = analogRead(MOTER_CURRENT_LINEAR);
  if(abs(current - 506) > 10) {
    DUMP_VAR(current);
  }
/*  
  bool turn = digitalRead(MOTER_FGS_WHEEL);
  //DUMP_VAR(turn);
  if(turn) {
    runMotorFGSignlCouter_NOT++;
  } else {
    //DUMP_VAR(turn);
    if(++runMotorFGSignlCouter > 9) {
      //DUMP_VAR(runMotorFGSignlCouter);
      //DUMP_VAR(runMotorFGSignlCouter_NOT);
      //DUMP_VAR(wheelRunCounter);
      wheelRunCounter = 0;
    }
  }
*/
/*  
  int turnAnalog = analogRead(MOTER_FGS_WHEEL);
  DUMP_VAR(turnAnalog);
  if(turnAnalog > 512) {
    DUMP_VAR(turnAnalog);
  }
*/
}

void checkOverRunLimit(void) {
  // stop
  if(wheelRunCounter-- <= 0 ) {
    STOP_WHEEL();
  }
  if(iDistanceWheelTof < iEROMWheelLimitBack) {
    bIsRunWheelByTof = false;
    STOP_WHEEL();
  }
  if(iDistanceWheelTof > iEROMWheelLimitFront) {
    bIsRunWheelByTof = false;
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
    if(InputCommand.length() > 256) {
      InputCommand = "";
    }
  }
}

void loop() {
  checkOverRunLimit();
  runSerialCommand();
  readStatus();
  readTof();
  calcWheel2TargetTof();
}

int const iTargetDistanceMaxDiff = 1;

int const aTofSpeedTable[] = {
  130,130,130,130,130,
  130,  130,  130  ,125,  0,
  0,  125,  130  ,130,  130,
  130,130,130,130,130,
};
int const aTofSpeedTableLength = sizeof(aTofSpeedTable)/aTofSpeedTable[0];

int const aTofSpeedTableHighResolution[] = {
  125,125,125,125,125,
  125,  125,  125  ,125,  0,
  0,  125,  125  ,125,  125,
  125,125,125,125,125,
};
int const aTofSpeedTableHighResolutionLength = sizeof(aTofSpeedTableHighResolution)/aTofSpeedTableHighResolution[0];


void calcWheel2TargetTof() {
  if(bIsRunWheelByTof == false) {
    return;
  }
  int diff = iTargetDistanceWheel - iDistanceWheelTof;
  if(abs(diff) < iTargetDistanceMaxDiff) {
    bIsRunWheelByTof = false;
    STOP_WHEEL();
    return;
  }
  int toMove = abs(diff);
  int moved = abs(iDistanceWheelTof - iStartDistanceWheel);
  int movedP = moved * 10 / iTotalDistanceWheel;
  DUMP_VAR(movedP);
  movedP %= aTofSpeedTableLength;
  DUMP_VAR(movedP);
  int speed = aTofSpeedTable[movedP];
  if(movedP > 8 && movedP < 12) {
    int movedP2 = moved * 100 / iTotalDistanceWheel;
    movedP2 -= 90;
    movedP2  %= aTofSpeedTableHighResolutionLength;
    DUMP_VAR(movedP2);
    speed = aTofSpeedTableHighResolution[movedP2];
  }
  DUMP_VAR(speed);
  if(diff > 0) {
    runWheel(speed,0);
  } else {
    runWheel(speed,1);
  }
}

