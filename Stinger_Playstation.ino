
#define DATA1 D2
#define CMD1 D3
#define ATT1 D5
#define CLK1 D6


#define JOYSTICK_DATA_SIZE 6
#define JOYSTICK_STATE_SIZE 4

//#define DEBUG

// http://problemkaputt.de/psx-spx.htm#controllerandmemorycardsignals
#define BDU !bitRead(data[0], 4) /* up */
#define BDD !bitRead(data[0], 6) /* down */
#define BDL !bitRead(data[0], 7) /* left */
#define BDR !bitRead(data[0], 5) /* right */
#define BST !bitRead(data[0], 3) /* start */
#define BSE !bitRead(data[0], 0) /* select */
#define BA !bitRead(data[1], 6) /* A */
#define BB !bitRead(data[1], 5) /* B */
#define BX !bitRead(data[1], 7) /* X */
#define BY !bitRead(data[1], 4) /* Y */
#define BLS !bitRead(data[1], 0) /* left shoulder */
#define BRS !bitRead(data[1], 1) /* right shoulder */  
#define BLT !bitRead(data[1], 2) /* left trigger */
#define BRT !bitRead(data[1], 3) /* right trigger */

inline void translateState(uint8_t *data, uint8_t *state) {
  state[0] = 0; state[1] = 0; state[2] = 0; state[3] = 0;
  if (BA) bitSet(state[0], 5);  //a 
  if (BB) bitSet(state[0], 4);  //b
  if (BST) bitSet(state[3], 1);  //select
  if (BSE) bitSet(state[3], 0);  //start
  if (BX) bitSet(state[0], 2);  //x
  if (BY) bitSet(state[3], 5);  //y
  if (BLT) bitSet(state[3], 3); //tl
  if (BRT) bitSet(state[3], 2); //tr
  if (BLS) bitSet(state[0], 3); //c
  if (BRS) bitSet(state[3], 4); //z
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
}

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
        translateState(data, state);
        flag = 1;
      }
    }

  }

  void sendState(uint8_t force = 0) {
    if (flag || force) {
      Serial.write(state, 4);
      flag = 0;
    }
  }

};


Joystick_ Joystick[1] =
{
    Joystick_(0)
};

//================================================================================
//================================================================================



uint8_t shift(uint8_t _dataOut) // Does the actual shifting, both in and out simultaneously
{
  uint8_t _temp = 0;
  uint8_t _dataIn = 0;

  delayMicroseconds(100); //max acknowledge waiting time 100us
  for (uint8_t _i = 0; _i <= 7; _i++) {
    
    if ( _dataOut & (1 << _i) ) // write bit
      digitalWrite(CMD1, HIGH);
    else 
      digitalWrite(CMD1, LOW);
    
    digitalWrite(CLK1, LOW); // read bit
    delayMicroseconds(10);
    _temp = digitalRead(DATA1);
    if (_temp) {
      _dataIn = _dataIn | (B00000001 << _i);
    }

    digitalWrite(CLK1, HIGH);
    delayMicroseconds(2);
  }
  return _dataIn;
}

void setup() {
  pinMode(DATA1, INPUT_PULLUP);
  pinMode(CMD1, OUTPUT);
  pinMode(ATT1, OUTPUT);
  pinMode(CLK1, OUTPUT);

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  #ifndef DEBUG
  Serial.begin(9600);
  #endif

}

void loop() {
  uint8_t head, padding, multitap;

  noInterrupts();
  digitalWrite(ATT1, LOW);
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
  interrupts();

  #ifdef DEBUG
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
  #ifndef DEBUG
  Joystick[0].sendState();
  #endif
   
  delayMicroseconds(1000);
}
