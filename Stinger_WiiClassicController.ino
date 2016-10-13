//Uno: SDA = A4, SCL = A5
//Pro Micro: SDA = D2, SCL = D3
//ESP8266: SDA = D2, SCL = D1

//#define LED 2

#include<Wire.h>

#define JOYSTICK_DATA_SIZE 6
#define JOYSTICK_STATE_SIZE 4

//http://wiibrew.org/wiki/Wiimote/Extension_Controllers/Classic_Controller
#define BDU !bitRead(data[5], 0) /* up */
#define BDD !bitRead(data[4], 6) /* down */
#define BDL !bitRead(data[5], 1) /* left */
#define BDR !bitRead(data[4], 7) /* right */
#define BST !bitRead(data[4], 2) /* start */
#define BSE !bitRead(data[4], 4) /* select */
#define BA !bitRead(data[5], 4) /* A */
#define BB !bitRead(data[5], 6) /* B */
#define BX !bitRead(data[5], 3) /* X */
#define BY !bitRead(data[5], 5) /* Y */
#define BLS !bitRead(data[5], 7) /* left shoulder */
#define BRS !bitRead(data[5], 2) /* right shoulder */  
#define BLT !bitRead(data[4], 5) /* left trigger */
#define BRT !bitRead(data[4], 1) /* right trigger */
//RT = (status[3] & (unsigned char)0x1f); //rightmost 5 bits
//LT = ((status[2] & (unsigned char)0x60) >> 2) + ((status[3] & (unsigned char)0xe0) >> 5); //'leftish' bits
//leftStickX = (status[0] & (unsigned char)0x3f); //rightmost 6 bits
//leftStickY = (status[1] & (unsigned char)0x3f); //rightmost 6 bits      
//rightStickX = ((status[0] & (unsigned char)0xc0) >> 3) + ((status[1] & (unsigned char)0xc0) >> 5) +  ((status[2] & (unsigned char)0x80) >> 7); //bits. They're there.
//rightStickY = (status[2] & (unsigned char)0x1f);    //rightmost 5 bits  
    
inline void translateState(uint8_t *data, uint8_t *state) {
  //https://github.com/torvalds/linux/blob/master/drivers/input/joystick/stinger.c
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


#define ADDRESS 0x52

void sendByte(int data, int location)  {  
  Wire.beginTransmission(ADDRESS);
  Wire.write(location);
  Wire.write(data);  
  Wire.endTransmission();
  delay(10);
}

void setup() {
  Serial.begin(9600);
  //pinMode(LED, OUTPUT);
  //digitalWrite(LED, HIGH);
  Wire.begin();
  sendByte(0x55, 0xF0);
  sendByte(0x00, 0xFB);
}

uint8_t data[JOYSTICK_DATA_SIZE];
uint8_t olddata[JOYSTICK_DATA_SIZE];
uint8_t state[JOYSTICK_STATE_SIZE];

void loop() {
  
  Wire.requestFrom(ADDRESS, JOYSTICK_DATA_SIZE); //request data from wii classic
  uint8_t i = 0;
  while(Wire.available()) {
    data[i] = Wire.read();
    i++;
    if (i >= JOYSTICK_DATA_SIZE) break;
  }

  //detect if init is needed
  if (i < JOYSTICK_DATA_SIZE) {
    delay(100);
    sendByte(0x55, 0xF0);
    sendByte(0x00, 0xFB);
    #ifdef DEBUG
    Serial.println("Init!");
    #endif
    delay(100);
  } else sendByte(0x00, 0x00);

  
  if (memcmp(olddata, data, JOYSTICK_DATA_SIZE)) {    
    memcpy(olddata, data, JOYSTICK_DATA_SIZE);
    translateState(data, state);
    Serial.write(state, 4);
    //Serial.flush();
  }

  const char *response = "\r\n0600520058C272";
  String s = "";
  if (Serial.available()) {
    if (Serial.read() == 0x20) {
      uint8_t i = 0;
      while (i < 4) {
        while (!Serial.available());
        char c = Serial.read();
        s += c;
        i++;
      }
    }
  }

  if (s == "E5E5") {
    Serial.write(response, 16);
    sendByte(0x55, 0xF0);
    sendByte(0x00, 0xFB);
    //digitalWrite(LED, LOW);
    //delay(2000);
    //digitalWrite(LED, HIGH);
  }

  delayMicroseconds(1000);
}

