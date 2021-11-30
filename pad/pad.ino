#include "Adafruit_NeoPixel.h"
volatile boolean received;
volatile byte Slavereceived,Slavesend;

#include "Adafruit_NeoPixel.h"
#define   Sensor_1_PIN  A1
#define   Sensor_2_PIN A0
#define   LEDPIN 4
#define   NUMPIXELS 68
#define   TRESHOLD 400
#define   RS_MODE_PIN 2
#define   SLAVE_CS 3
#define   PAD_ID_1 9
#define   PAD_ID_2 8
#define   PAD_ID_3 7
#define   PAD_ID_4 6

#define PAD_ID 10

#define INTENSIVITY_0  90
#define INTENSIVITY_1  150
#define INTENSIVITY_2  200

#define RED 1
#define GREEN 2
#define BLUE 3
#define REACTION_RED 4
#define REACTION_BLUE 5
#define REACTION_GREEN 6
#define DISABLE_REACTION 7
#define OFF 8
#define SET_INTENSITIVITY_0 9
#define SET_INTENSITIVITY_1 10
#define SET_INTENSITIVITY_2 11
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
byte readout = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
byte led_intensitivity = INTENSIVITY_1;
void enter_transmit_mode(){
  digitalWrite(RS_MODE_PIN,HIGH);
}

void enter_recive_mode(){
  Serial.flush();
  digitalWrite(RS_MODE_PIN,LOW);
}

void setup()

{ 
  pinMode(PAD_ID_1,INPUT_PULLUP);
  pinMode(PAD_ID_2,INPUT_PULLUP);
  pinMode(PAD_ID_3,INPUT_PULLUP);
  pinMode(PAD_ID_4,INPUT_PULLUP);
  byte a1 = (byte)digitalRead(PAD_ID_1);
  byte a2 = (byte)digitalRead(PAD_ID_2);
  byte a3 = (byte)digitalRead(PAD_ID_3);
  byte a4 = (byte)digitalRead(PAD_ID_4);
  padID = 1*a1+2*a2+4*a3+8*a4;
  pixels.begin();
  pinMode(RS_MODE_PIN,OUTPUT);
  enter_recive_mode();
  digitalWrite(RS_MODE_PIN,LOW);
  pinMode(13,OUTPUT);   //DEBUG LED
  Serial.begin(250000);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  for(int i = 0 ; i < padID; i++){
    pixels.setPixelColor(i+1, pixels.Color(0,0,led_intensitivity));                                              
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
      control_lights(1,led_intensitivity,0,0);
         //digitalWrite(13,HIGH);
      break;
    case GREEN:
       control_lights(1,0,led_intensitivity,0);
       break;
    case BLUE:
       control_lights(1,0,0,led_intensitivity);
       break;
    case WHITE:
       control_lights(1,led_intensitivity,led_intensitivity,led_intensitivity);
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
    case SET_INTENSITIVITY_0:
          led_intensitivity = INTENSIVITY_0;
        break;
    case SET_INTENSITIVITY_1:
          led_intensitivity = INTENSIVITY_1;
        break;
    case SET_INTENSITIVITY_2:
          led_intensitivity = INTENSIVITY_2;
        break;
    case REACTION_CHANGE:
          reaction_change = true;
        break;
    case RAPORT_READOUT:
          enter_transmit_mode();
            Serial.write(13);
            byte readout = normalized_sensor_read();
            Serial.write(readout);
            checksum_tmp=13^readout;
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
  int out = readout/4;
  return((byte)out);
}
