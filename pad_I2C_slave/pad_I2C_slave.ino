/* Code for Zapurex Pad Slave
 *  
 * Atmega currently handles 2 tasks: 
 *  - Measuer tnsion on the tension metter.
 *  - Controll RGB LED lights onthe pad
 *  
 *  Amega is connected to ESP32 via I2C interface. 
 *  Communication follows the frame format below:
 *  | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte   |
 *  |--------|--------|--------|--------|--------|----------|
 *  |function|  mode  |    R   |   G    |   B    | checksum |
 *  
 *  Available modes:
 *    - 0x00 turn the lights on
 *    - 0x01 turn the lgihts off
 *    - 0x02 brighten
 *    - 0x03 fade
 *    - 0x04 breath
 *    - 0x05 flash 
 *    
 */
#include <Wire.h>
#include "Adafruit_NeoPixel.h"

//#define DEBUG -//debug mode for Serial communication.
// Specify LED RGB outputs
#define   I2C_Address 8
#define   SOFT_VERSION  1
#define   Sensor_1_PIN  A6
#define   Sensor_2_PIN A7
#define   LEDPIN 6
#define   NUMPIXELS 144
#define   TRESHOLD 500

int Read_1;   
int Read_2; 
bool pad_hit = false; 

char out_buffer[4];
byte frame[6];
byte Reaction_R =0;
byte Reaction_G =0;
byte Reaction_B =0;
bool reaction = false;
int counter = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

void setup() {

  pixels.begin();
  Wire.begin(I2C_Address);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  
  #ifdef DEBUG
  Serial.begin(9600);           // start serial for output
  #endif
  pinMode(Sensor_1_PIN,INPUT);
  pinMode(Sensor_2_PIN,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
         out_buffer[0] = 100;
         out_buffer[1] = 101;
         out_buffer[2] = 102;
         out_buffer[3] = 99;
  }

void loop() {

  Read_1 = analogRead(Sensor_1_PIN);
  Read_2 = analogRead(Sensor_2_PIN);
  if(reaction && Read_2 > TRESHOLD || Read_1 > TRESHOLD){
    control_lights(0,Reaction_R,Reaction_G,Reaction_B);
    delay(3000);
    control_lights(1,0,0,0);
  }
}

void handle_message(byte frame[5]) {
  byte checksum = 0;
  for(int kk = 0; kk < 4; kk++) checksum = frame[kk] ^ checksum;
  checksum += 4;
  switch (frame[0]) {
      break;
    case 0x0C:
       control_lights(0, frame[1], frame[2], frame[3]);
      break;
    case 0x0B:
       control_lights(1, frame[1], frame[2], frame[3]);
       break;
    case 0xAA:
       digitalWrite(LED_BUILTIN,HIGH);
       break;
    case 0xBB:
        digitalWrite(LED_BUILTIN,LOW);
        break;
    case 0xCC:
         out_buffer[0] = 69;
         out_buffer[1] = 70;
         out_buffer[2] = 127;
         out_buffer[3] = 90;
        break;
    case 0xAD:
          Reaction_R = frame[1];
          Reaction_G = frame[2];
          Reaction_B = frame[3];
          reaction = true;
      break;
    default: 
     #ifdef DEGUB
      Serial.println("I2C error");
     #endif
      break;
  }
  counter = 0;
}
//}

void control_lights(byte mode, byte R, byte G, byte B) {
  #ifdef DEBUG
  Serial.print("Mode is: "); Serial.println(mode);
  #endif
  switch (mode) {
    case 0x00:
      lights_up(R, G, B);
      break;
    case 0x01:
      lights_down();
      break;
     default:
      break;
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    frame[counter] = Wire.read();
    #ifdef DEBUG
    Serial.print(counter); Serial.print(" : ");Serial.println(frame[counter]);
    #endif
    counter++;
  }
  handle_message(frame);
}

// function that executes whenever data is requested by master
void requestEvent() {
    Wire.write(out_buffer);
}

void lights_up(byte R, byte G, byte B) {
    for(int i = 0 ; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(R,G,B));                                              
    }
    pixels.show(); 
}

void lights_down() {
   for(int i = 0 ; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));                                              
    }
    pixels.show();
}

