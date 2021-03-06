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
#define   RS_MODE_PIN 2
#define   SLAVE_CS 3
/* TUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU KURRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRWWWWWWWWWWWWWWWWWWWWWWAAAAAAAAAAAAAAAAAAA TUUUTUTTTTTTTTTTTTTTTTTTTTTTTTTTTT*/
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
#define TRESHHOLD_SET 17
#define RAPORT_READOUT 18
#define READ_TEST 19
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
byte msg[3],recovery_tmp[5];
byte i_counter = 0;
byte checksum_tmp = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

void enter_transmit_mode(){
  digitalWrite(RS_MODE_PIN,HIGH);
}

void enter_recive_mode(){
  Serial.flush();
  digitalWrite(RS_MODE_PIN,LOW);
}

void setup()

{ 
  pixels.begin();

  pinMode(RS_MODE_PIN,OUTPUT);
 //enter_recive_mode();
  digitalWrite(RS_MODE_PIN,LOW);
  pinMode(13,OUTPUT);   //DEBUG LED
  
 /* for(int yy = 0; yy< padID;yy++){
    digitalWrite(13,HIGH);
    delay(20);
    digitalWrite(13,LOW);
    delay(20);
  }
  */

 // lights_up(255,255,255);
  //delay(200);
  //lights_down();
  Serial.begin(250000);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  for(int i = 0 ; i < PAD_ID; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,255));                                              
  }
    pixels.show(); 

}

void back_to_current(){
  control_lights(true,current_R,current_G,current_B);
}

void MasterMSGCheck(){
if(Serial.available() > 0){
   while(Serial.available() >0){
      msg[counter] = Serial.read();
      counter++;
      if(counter == 3){
        counter=0;      
        if(checksum()){
        if(msg[0] == padID || msg[0] == 0)handle_message(msg[1]);
      }else{
        get_back_on_track();
      }
     }
   }
  }
}

void get_back_on_track(){
  byte jj = 0;
  while(jj<5){
    if(Serial.available() >0){
      recovery_tmp[jj] = Serial.read();
      jj++;
      if(jj == 4 && (recovery_tmp[1]^recovery_tmp[2]) == (recovery_tmp[3]-4)){
        
        break;
      }
      if(jj == 5 && (recovery_tmp[2]^recovery_tmp[3]) == (recovery_tmp[4]-4)){
        
        break;
      }
    }
    digitalWrite(13,HIGH);
    counter = 0;
  }
}

bool checksum(){
  byte tmp =msg[0]^msg[1];
  tmp+=4;
  if(tmp == msg[2])return(true);
  return(false);
}

void loop(){
  MasterMSGCheck();
 // Read_1 = analogRead(Sensor_1_PIN);
  //Read_2 = analogRead(Sensor_2_PIN);
 /* if(Read_2 > treshold || Read_1 > treshold){
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
  }    */  
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
         //digitalWrite(13,HIGH);
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
          //digitalWrite(PROMPT_PIN,LOW);
        break;
    case DISABLE_REACTION:
          reaction = false;
          //digitalWrite(PROMPT_PIN,LOW);
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
    case RAPORT_READOUT:
          enter_transmit_mode();
            Serial.write(13);
            checksum_tmp = normalized_sensor_read();
            Serial.write(checksum_tmp);
            checksum_tmp=13^checksum_tmp;
            checksum_tmp+=4;
            Serial.write(checksum_tmp);
          enter_recive_mode();
        break;  
    case READ_TEST:
          digitalWrite(13,HIGH);
          enter_transmit_mode();
          Serial.write(69);
          enter_recive_mode();
        break;
    default: 
          treshold = frame;
      break;
  }
}

byte normalized_sensor_read(){
  int readout = analogRead(Sensor_2_PIN);
  if(readout > 255)readout = 255;
  byte out = readout;
  return(out);
}
