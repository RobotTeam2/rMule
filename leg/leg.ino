#include <ArduinoJson.h>


const static char MOTER_PWM_WHEEL = 12;
const static char MOTER_CCW_WHEEL = 27;
const static char MOTER_FGS_WHEEL = 29;
unsigned char speed_wheel = 0x0;

static long wheelRunCounter = -1;
static long const iRunTimeoutCounter = 30000L * 1L;



#define FRONT() { \
  digitalWrite(MOTER_CCW_WHEEL, LOW);\
  }


#define BACK() { \
  digitalWrite(MOTER_FGS_WHEEL, HIGH);\
  }

#define STOP() {\
  speed_wheel =0x00;\
  analogWrite(MOTER_PWM_WHEEL, 0x0);\
}


void setup()
{
  pinMode(MOTER_PWM_WHEEL, OUTPUT);
  pinMode(MOTER_CCW_WHEEL, OUTPUT);
  pinMode(MOTER_FGS_WHEEL, INPUT);
  
  

  TCCR1B = (TCCR1B & 0b11111000) | 0x01;
  
  STOP();

//  Serial.begin(9600);
  Serial.begin(115200);

  Serial.print("start rMule leg\n");\

}

String InputCommand ="";



#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  Serial.print(x);\
  Serial.print(">\n");\
}


void runWheel(int spd,int front) {
  speed_wheel = spd;
  analogWrite(MOTER_PWM_WHEEL, spd);
  wheelRunCounter = iRunTimeoutCounter;
  if(front) {
    digitalWrite(MOTER_CCW_WHEEL , HIGH);
    DUMP_VAR(front);
  } else {
    digitalWrite(MOTER_CCW_WHEEL, LOW);
    DUMP_VAR(front);
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
    int spd = params["s"];
    int front = params["f"];
    DUMP_VAR(motor);
    DUMP_VAR(spd);
    DUMP_VAR(front);
    //CONFIRM_WHEEL();
    if(motor == "wheel") {
      runWheel(spd,front);
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
}

