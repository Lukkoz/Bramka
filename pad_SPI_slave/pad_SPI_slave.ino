#include <SPI.h>
#include "Adafruit_NeoPixel.h"
volatile boolean received;
volatile byte Slavereceived,Slavesend;

#include "Adafruit_NeoPixel.h"
#define   Sensor_1_PIN  A1
#define   Sensor_2_PIN A0
#define   LEDPIN 5
#define   NUMPIXELS 144
#define   TRESHOLD 400
#define   PROMPT_PIN 4
#define   SLAVE_CS 3

#define PAD_ID 12

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
#define REACTION_CHANGE 13
#define WHITE 14
#define TRESHOLD_UP 15
#define TRESHOLD_DOWN 16
#define TRESHHOLD_SET 17
int Read_1;   
int Read_2; 
int treshold = TRESHOLD;
bool pad_hit = false; 
long reactionTime = 1000;
long hitTime;
byte padID = PAD_ID;
byte frame;
byte Reaction_R =0;
byte Reaction_G =255;
byte Reaction_B =0;
byte current_R =0;
byte current_G =0;
byte current_B =255;
bool reaction = false;
bool reaction_change = false;
int counter = 0;
bool hitSignal = false;
byte msg[3];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);


void setup()

{ 
  delay(2000);
  pixels.begin();
  pinMode(Sensor_1_PIN,INPUT);
  pinMode(Sensor_2_PIN,INPUT);

  pinMode(4,OUTPUT);                
  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 
  pinMode(PROMPT_PIN,OUTPUT);
  digitalWrite(PROMPT_PIN,LOW);
  pinMode(13,OUTPUT);
  
  for(int yy = 0; yy< padID;yy++){
    digitalWrite(13,HIGH);
    delay(200);
    digitalWrite(13,LOW);
    delay(200);
  }

  lights_up(255,255,255);
  delay(200);
  lights_down();
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);

}

void back_to_current(){
  control_lights(true,current_R,current_G,current_B);
}

void MasterMSGCheck(){
if(Serial.available() > 0){
   while(Serial.available() >0){
      msg[counter] = Serial.read();
      counter++;
      if(counter == 3 && msg[1] != TRESHHOLD_SET){
        counter=0;      
        if(checksum()){
        if(msg[0] == padID || msg[0] == 0)handle_message(msg[1]);
      }
      if(msg[1] == TRESHHOLD_SET && counter == 4){
         if(msg[0]^msg[1]^msg[2] == msg[3]){
          if(msg[0] == padID || msg[0] == 0){
            treshold = 4095.0*float(msg[3]/100);
             for(int yy = 0; yy< treshold;yy++){
                digitalWrite(13,HIGH);
                delay(200);
                digitalWrite(13,LOW);
                delay(200);
             }
          } 

         }
      }
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
  if(Read_2 > treshold || Read_1 > treshold){
    if(reaction && !hitSignal){
      control_lights(reaction_change,Reaction_R,Reaction_G,Reaction_B);
      hitSignal = true;
      hitTime = millis();
    }  
  }  
  if(hitSignal && reactionTime != 0){
    if((millis()-hitTime) > reactionTime){
      back_to_current();
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

void control_lights(bool flag, byte R, byte G, byte B) {
      lights_up(R, G, B);
      if(flag == true){
        current_B = B;
        current_G = G;
        current_R = R;
      }
}

void handle_message(byte frame) {
  byte checksum = 0;
  switch (frame) {
      break;
    case RED:
       control_lights(1,255,0,0);
         digitalWrite(13,HIGH);
      break;
    case GREEN:
       control_lights(1,0,255,0);
       break;
    case BLUE:
       control_lights(1,0,0,255);
       break;
    case WHITE:
       control_lights(1,255,255,255);
    break;
     case OFF:
       control_lights(1,0,0,0);
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
           reaction_change = false;
        break;
    case REACTION_TIME_2000:
          reactionTime = 2000;
           reaction_change = false;
        break;
    case REACTION_TIME_500:
          reactionTime = 500;
           reaction_change = false;
        break;
    case REACTION_CHANGE:
          reaction_change = true;
        break;
    case TRESHOLD_UP:
          treshold = 50;
       break;
    case TRESHOLD_DOWN:
          treshold = 10;
       break; 
    default: 
          treshold = frame;
      break;
  }
}
