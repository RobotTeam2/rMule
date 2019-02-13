#include <ArduinoJson.h>
#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;

#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  Serial.print(x);\
  Serial.print(">\n");\
}


const static char MOTER_PWM_WHEEL = 12;
const static char MOTER_CCW_WHEEL = 27;
const static char MOTER_FGS_WHEEL = 29;



const static char MOTER_CURRENT_LINEAR = 2;
const static char MOTER_PWM_LINEAR = 3;
const static char MOTER_STANDBY_LINEAR = 5;
const static char MOTER_A1_LINEAR = 7;
const static char MOTER_A2_LINEAR = 9;



unsigned char speed_wheel = 0x0;

static long wheelRunCounter = -1;
static long const iRunTimeoutCounter = 10000L * 3L;



#define FRONT() { \
  digitalWrite(MOTER_CCW_WHEEL, LOW);\
  }


#define BACK() { \
  digitalWrite(MOTER_CCW_WHEEL, HIGH);\
  }

#define STOP() {\
  speed_wheel =0x00;\
  analogWrite(MOTER_PWM_WHEEL, 0x0);\
}

//#define HIGH_SPEED
//#define HIGH_ACCURACY

const int TOP_TIME_OUT = 1;

void setupTof() {
  Wire.begin();
  sensor.init();
  sensor.setTimeout(TOP_TIME_OUT);
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

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif
}

int prevDistance = 0;
const int iDistanceDiffMax = 10;
void readTof() {
  //DUMP_VAR("");
  int distance = sensor.readRangeSingleMillimeters();
  //int distance = 0;
  if(distance <= 0 || distance >= 8191) {
    return ;
  }
  if(abs(distance - prevDistance) > iDistanceDiffMax) {
    //DUMP_VAR(distance);
    StaticJsonDocument<256> doc;
    JsonObject root = doc.to<JsonObject>();
    root["tofw"] = distance;
    String output;
    serializeJson(doc, output);
    Serial.print(output);
    Serial.print("\n");
  }
  prevDistance = distance;
}


void setup()
{
  pinMode(MOTER_PWM_WHEEL, OUTPUT);
  pinMode(MOTER_CCW_WHEEL, OUTPUT);
  pinMode(MOTER_FGS_WHEEL, INPUT);
  //pinMode(MOTER_FGS_WHEEL, INPUT_PULLUP);
  
  

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
  
  STOP();

//  Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg\n");\

  setupTof();
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
    DUMP_VAR(front);
  } else {
    digitalWrite(MOTER_CCW_WHEEL, LOW);
    DUMP_VAR(front);
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
    String motor = kv.key().c_str();
    DUMP_VAR(motor);
    //CONFIRM_WHEEL();
    if(motor == "wheel" || motor == "W") {
      int spd = params["s"];
      int front = params["f"];
      DUMP_VAR(spd);
      DUMP_VAR(front);
      runWheel(spd,front);
    }
    if(motor == "linear" || motor == "L") {
      int distance = params["d"];
      int ground = params["g"];
      DUMP_VAR(distance);
      DUMP_VAR(ground);
      runLinear(distance,ground);
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
    FRONT();
    wheelRunCounter = iRunTimeoutCounter;
  }
  if(InputCommand=="bb") {
    BACK();
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
  bool turn = digitalRead(MOTER_FGS_WHEEL);
  //DUMP_VAR(turn);
  if(turn) {
    runMotorFGSignlCouter_NOT++;
  } else {
    //DUMP_VAR(turn);
    if(++runMotorFGSignlCouter > 9) {
      DUMP_VAR(runMotorFGSignlCouter);
      DUMP_VAR(runMotorFGSignlCouter_NOT);
      DUMP_VAR(wheelRunCounter);
      wheelRunCounter = 0;
    }
  }
/*  
  int turnAnalog = analogRead(MOTER_FGS_WHEEL);
  DUMP_VAR(turnAnalog);
  if(turnAnalog > 512) {
    DUMP_VAR(turnAnalog);
  }
*/
}


void loop() {
  // stop
  if(wheelRunCounter-- <= 0) {
    STOP();
  }
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
  readStatus();
  readTof();
}


