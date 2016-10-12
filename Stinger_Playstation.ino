
#define DATA1 D2
#define CMD1 D3
#define ATT1 D5
#define CLK1 D6

/*#define DATA2 6
#define CMD2 7
#define ATT2 8
#define CLK2 9 */


#define JOYSTICK_DATA_SIZE 6
#define JOYSTICK_STATE_SIZE 4

//#define DEBUG

//================================================================================
//================================================================================
//  Joystick (Gamepad)


uint8_t state[JOYSTICK_STATE_SIZE];

class Joystick_ {

private:
  uint8_t joystickId;
  uint8_t olddata[JOYSTICK_DATA_SIZE];
  uint8_t flag;

public:
  uint8_t type;
  uint8_t data[JOYSTICK_DATA_SIZE];

  Joystick_(uint8_t initJoystickId) {
    // Initalize State
    joystickId = initJoystickId;
  
    data[0] = 0;
    data[1] = 0;
    data[2] = 127;
    data[3] = 127;
    data[4] = 127;
    data[5] = 127;
    memcpy(olddata, data, JOYSTICK_DATA_SIZE);
    sendState(1);
  }

  void updateState() {
    if (type != 0x73 && type != 0x53) {
      data[2] = 127;
      data[3] = 127;
      data[4] = 127;
      data[5] = 127;
    }
    if (type == 0x41 || type == 0x73 || type == 0x53) {
      if (memcmp(olddata, data, JOYSTICK_DATA_SIZE)) {    
        memcpy(olddata, data, JOYSTICK_DATA_SIZE);
        flag = 1;
      }
    }
    //sendState();
  }

  void sendState(uint8_t force = 0) {
    if (flag || force) {
      // HID().SendReport(Report number, array of values in same order as HID descriptor, length)
      //HID().SendReport(reportId, data, JOYSTICK_STATE_SIZE);
      Serial.write(state[0]);
      Serial.write(state[1]);
      Serial.write(state[2]);
      Serial.write(state[3]);
      /*Serial.write(0x5a);
      Serial.write(0xfe);
      Serial.write(0x01);
      Serial.write(0xaa);*/
      flag = 0;
    }
  }

};


Joystick_ Joystick[4] =
{
    Joystick_(0),
    Joystick_(1),
    Joystick_(2),
    Joystick_(3)
};

//================================================================================
//================================================================================





uint8_t shift(uint8_t _dataOut) // Does the actual shifting, both in and out simultaneously
{
  uint8_t _temp = 0;
  uint8_t _dataIn = 0;
  uint8_t _delay = 10; //ESP8266: 10; //5V Arduino: 6; //2 unstable; //clock 250kHz

  delayMicroseconds(100); //max acknowledge waiting time 100us
  for (uint8_t _i = 0; _i <= 7; _i++) {
    
    if ( _dataOut & (1 << _i) ) // write bit
      digitalWrite(CMD1, HIGH);
    else 
      digitalWrite(CMD1, LOW);
    
    digitalWrite(CLK1, LOW); // read bit
    delayMicroseconds(_delay);
    _temp = digitalRead(DATA1);
    if (_temp) {
      _dataIn = _dataIn | (B00000001 << _i);
    }

    digitalWrite(CLK1, HIGH);
    delayMicroseconds(_delay);
  }
  return _dataIn;
}

void setup() {
  pinMode(DATA1, INPUT_PULLUP);
  pinMode(CMD1, OUTPUT);
  pinMode(ATT1, OUTPUT);
  pinMode(CLK1, OUTPUT);

  /*pinMode(DATA2, INPUT_PULLUP);
  pinMode(CMD2, OUTPUT);
  pinMode(ATT2, OUTPUT);
  pinMode(CLK2, OUTPUT);*/
  
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  Serial.begin(9600);

}

void loop() {
  // http://problemkaputt.de/psx-spx.htm#controllerandmemorycardsignals
  uint8_t head, padding, multitap;
  #ifdef DEBUG
  uint8_t data[100];
  #endif

  // first: read gamepad normally
  digitalWrite(ATT1, LOW);
  //digitalWrite(ATT2, LOW);
  head = shift(0x01);
  Joystick[0].type = shift(0x42);
  padding = shift(0x01); //read multitap in next command
  Joystick[0].data[0] = shift(0x00); //buttons
  Joystick[0].data[1] = shift(0x00); //buttons
  Joystick[0].data[2] = shift(0x00); //right analog
  Joystick[0].data[3] = shift(0x00); //right analog
  Joystick[0].data[4] = shift(0x00); //left analog
  Joystick[0].data[5] = shift(0x00); //left analog
  digitalWrite(ATT1, HIGH);
  //digitalWrite(ATT2, HIGH);

  //delay(100);

/*  // second: check and read multitap
  digitalWrite(ATT1, LOW);
  head = shift(0x01);
  multitap = shift(0x42);
  padding = shift(0x00); //next time normal read
  if (multitap == 0x80) {
    for (uint8_t i = 0; i < 4; i++) {
      Joystick[i].type = shift(0x00);
      padding = shift(0x00);
      Joystick[i].data[0] = shift(0x00); //buttons
      Joystick[i].data[1] = shift(0x00); //buttons
      Joystick[i].data[2] = shift(0x00); //right analog
      Joystick[i].data[3] = shift(0x00); //right analog
      Joystick[i].data[4] = shift(0x00); //left analog
      Joystick[i].data[5] = shift(0x00); //left analog
    }
  }
  digitalWrite(ATT1, HIGH);*/

  uint8_t BDU, BDD, BDL, BDR, BA, BB, BM, BP, BX, BY, BLT, BRT, BZL, BZR;
  BDU = !bitRead(Joystick[0].data[0], 4);  //up
  BDD = !bitRead(Joystick[0].data[0], 6);  //down
  BDL = !bitRead(Joystick[0].data[0], 7);  //left
  BDR = !bitRead(Joystick[0].data[0], 5);  //right
  BA = !bitRead(Joystick[0].data[1], 6);  //A
  BB = !bitRead(Joystick[0].data[1], 5);  //B
  BM = !bitRead(Joystick[0].data[0], 0);  //-/select
  BP = !bitRead(Joystick[0].data[0], 3);  //+/start
  BX = !bitRead(Joystick[0].data[1], 7);  //X
  BY = !bitRead(Joystick[0].data[1], 4);  //Y
  BLT = !bitRead(Joystick[0].data[1], 2);  //LT
  BRT = !bitRead(Joystick[0].data[1], 3);  //RT
  BZL = !bitRead(Joystick[0].data[1], 0);  //ZL
  BZR = !bitRead(Joystick[0].data[1], 1);  //ZR  
  
  state[0] = 0; state[1] = 0; state[2] = 0; state[3] = 0;
  if (BA) bitSet(state[0], 5);  //a 
  if (BB) bitSet(state[0], 4);  //b
  if (BM) bitSet(state[3], 1);  //select
  if (BP) bitSet(state[3], 0);  //start
  if (BX) bitSet(state[0], 2);  //x
  if (BY) bitSet(state[3], 5);  //y
  if (BLT) bitSet(state[3], 3); //tl
  if (BRT) bitSet(state[3], 2); //tr
  if (BZL) bitSet(state[0], 3); //c
  if (BZR) bitSet(state[3], 4); //z
  if (BDU) {
    state[2] = B00111111; //-32767
    bitClear(state[0], 1);
  }  
  if (BDD) {
    state[2] = B00000000; //32767
    bitSet(state[0], 1);
  }
  if (BDL) {
    state[1] = B00000000; //-32767
    bitSet(state[0], 0);
  }
  if (BDR) {
    state[1] = B00111111; //32767
    bitClear(state[0], 0);
  }
  
  #ifdef DEBUG
  /*for (uint8_t i = 0; i < 4; i++) {
    Serial.print(" multitap: "); Serial.println(multitap, HEX);
    Serial.print(" type: 0x"); Serial.print(Joystick[i].type, HEX);
    Serial.print(" data: 0x"); Serial.print(Joystick[i].data[0], HEX);
    Serial.print(" 0x"); Serial.print(Joystick[i].data[1], HEX);
    Serial.print(" 0x"); Serial.print(Joystick[i].data[2], HEX);
    Serial.print(" 0x"); Serial.print(Joystick[i].data[3], HEX);
    Serial.print(" 0x"); Serial.print(Joystick[i].data[4], HEX);
    Serial.print(" 0x"); Serial.print(Joystick[i].data[5], HEX);
    Serial.println();
  }*/
  Serial.print(" type: 0x"); Serial.print(Joystick[0].type, HEX);
  Serial.print(" data: 0x"); Serial.print(Joystick[0].data[0], HEX);
  Serial.print(" 0x"); Serial.print(Joystick[0].data[1], HEX);
  Serial.print(" 0x"); Serial.print(Joystick[0].data[2], HEX);
  Serial.print(" 0x"); Serial.print(Joystick[0].data[3], HEX);
  Serial.print(" 0x"); Serial.print(Joystick[0].data[4], HEX);
  Serial.print(" 0x"); Serial.print(Joystick[0].data[5], HEX);
  Serial.println();
  
  Serial.flush();
  #endif

  Joystick[0].updateState();
  //Joystick[1].updateState();
  //Joystick[2].updateState();
  //Joystick[3].updateState();
  Joystick[0].sendState();
  //Joystick[1].sendState();
  //Joystick[2].sendState();
  //Joystick[3].sendState();  
  delayMicroseconds(1000);


}

