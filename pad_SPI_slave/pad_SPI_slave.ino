#include <SPI.h>
#include "Adafruit_NeoPixel.h"
volatile boolean received;
volatile byte Slavereceived,Slavesend;

#include "Adafruit_NeoPixel.h"
#define   Sensor_1_PIN  A1
#define   Sensor_2_PIN A0
#define   LEDPIN 5
#define   NUMPIXELS 144
#define   TRESHOLD 500
#define   PROMPT_PIN 4
#define   SLAVE_CS 3

#define RED 1
#define GREEN 2
#define BLUE 3
#define REACTION_RED 4
#define REACTION_BLUE 5
#define REACTION_GREEN 6
#define DISABLE_REACTION 7
#define OFF 8
#define REACTION_TIME_1000 9
#define REACTION_TIME_2000 10
#define REACTION_TIME_500 11
#define CLEAR_HIT_SIGNAL 12

int Read_1;   
int Read_2; 
bool pad_hit = false; 
long reactionTime = 1000;
long hitTime;
byte padID = 4;

char out_buffer[4];
byte frame;
byte Reaction_R =0;
byte Reaction_G =0;
byte Reaction_B =0;

byte current_R = 0;
byte current_G = 0;
byte current_B = 0;
bool reaction = false;
int counter = 0;
bool hitSignal = false;
byte msg[3];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
void setup()

{
  Serial.begin(9600);
  pixels.begin();
  pinMode(Sensor_1_PIN,INPUT);
  pinMode(Sensor_2_PIN,INPUT);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);

  digitalWrite(PROMPT_PIN,LOW);
  pinMode(13,OUTPUT);
  lights_up(0, 0, 255);
  digitalWrite(13,HIGH);
  delay(200);
  lights_up(255,0,0);
  digitalWrite(13,LOW);
  delay(200);
  digitalWrite(13,HIGH);
}


void MasterMSGCheck(){
if(Serial.available() > 0){
   while(Serial.available() >0){
      msg[counter] = Serial.read();
      counter++;
      if(counter == 3 && checksum()){
        if(msg[0] == padID)handle_message(msg[1]);
        counter = 0;
      }
   }
  }
}

bool checksum(){
  if(msg[0]^msg[1] == msg[2])return(true);
  return(false);
}

void loop(){
MasterMSGCheck();
  Read_1 = analogRead(Sensor_1_PIN);
  Read_2 = analogRead(Sensor_2_PIN);
  if(Read_2 > TRESHOLD || Read_1 > TRESHOLD){
    if(reaction)control_lights(0,Reaction_R,Reaction_G,Reaction_B);
    hitSignal = true;
    digitalWrite(PROMPT_PIN,HIGH);
    hitTime = millis();
  }  
  if(reaction && hitSignal && reactionTime != 0){
    if((millis()-hitTime) > reactionTime){
      control_lights(0,current_R,current_G,current_B);
      hitSignal = false;
    }
  }      
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

void control_lights(byte mode, byte R, byte G, byte B) {
  switch (mode) {
    case 0x00:
      lights_up(R, G, B);
       digitalWrite(13,HIGH);
      break;
    case 0x01:
      lights_down();
       digitalWrite(13,LOW);
      break;
     default:
      break;
  }
}

void handle_message(byte frame) {
  byte checksum = 0;
 // for(int kk = 0; kk < 4; kk++) checksum = frame[kk] ^ checksum;
  //checksum += 4;
  switch (frame) {
      break;
    case RED:
       control_lights(0,255,0,0);
       digitalWrite(13,HIGH);
        current_R = 255;
        current_G = 0;
        current_B = 0;
      break;
    case GREEN:
       control_lights(0,0,255,0);
        current_R = 0;
        current_G = 255;
        current_B = 0;
       break;
    case BLUE:
       control_lights(0,0,0,255);
        current_R = 0;
        current_G = 0;
        current_B = 255;
       break;
     case OFF:
       control_lights(0,0,0,0);
        current_R = 0;
        current_G = 0;
        current_B = 0;
        digitalWrite(13,LOW);
      break;
    case REACTION_RED:
          Reaction_R = 255;
          Reaction_G = 0;
          Reaction_B = 0;
          reaction = true;
        break;
    case REACTION_GREEN:
          Reaction_R = 0;
          Reaction_G = 255;
          Reaction_B = 0;
          reaction = true;
        break;
    case REACTION_BLUE:
          Reaction_R = 0;
          Reaction_G = 0;
          Reaction_B = 255;
          reaction = true;
        break;
    case CLEAR_HIT_SIGNAL:
          hitSignal = false;
          digitalWrite(PROMPT_PIN,LOW);
        break;
    case DISABLE_REACTION:
          reaction = false;
          digitalWrite(PROMPT_PIN,LOW);
        break;
    case REACTION_TIME_1000:
          reactionTime = 1000;
        break;
    case REACTION_TIME_2000:
          reactionTime = 2000;
        break;
    case REACTION_TIME_500:
          reactionTime = 500;
        break;
    default: 
      break;
  }
}
