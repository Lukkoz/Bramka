#include <SPI.h>
#include "Adafruit_NeoPixel.h"
volatile boolean received;
volatile byte Slavereceived,Slavesend;

#include "Adafruit_NeoPixel.h"
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

ISR (SPI_STC_vect)                        //Inerrrput routine function 
{
  receiveSPIEvent();         // Value received from master if store in variable slavereceived 
  //Sets received as True 
}

void receiveSPIEvent() {
   // loop through all but the last
    frame[counter] = SPDR;
    #ifdef DEBUG
    Serial.print(counter); Serial.print(" : ");Serial.println(frame[counter]);
    #endif
    counter++;
    if(counter == 4)handle_message(frame);
}

void setup()

{
  Serial.begin(115200);
  
  pixels.begin();
  pinMode(Sensor_1_PIN,INPUT);
  pinMode(Sensor_2_PIN,INPUT);

         out_buffer[0] = 100;
         out_buffer[1] = 101;
         out_buffer[2] = 102;
         out_buffer[3] = 99;

  pinMode(4,OUTPUT);                
  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 

  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  SPCR |= _BV(SPIE);
  received = false;
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder (MSBFIRST);
  lights_up(0, 0, 255);
  delay(200);
  lights_up(255,0,0);
}



void loop(){

  /*Read_1 = analogRead(Sensor_1_PIN);
  Read_2 = analogRead(Sensor_2_PIN);
  if(reaction && Read_2 > TRESHOLD || Read_1 > TRESHOLD){
    control_lights(0,Reaction_R,Reaction_G,Reaction_B);
    delay(3000);
    control_lights(1,0,0,0);
  }  
  */    

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

void handle_message(byte frame[5]) {
  byte checksum = 0;
 // for(int kk = 0; kk < 4; kk++) checksum = frame[kk] ^ checksum;
  //checksum += 4;
  switch (frame[0]) {
      break;
    case 0x0C:
       control_lights(0, frame[1], frame[2], frame[3]);
      break;
    case 0x0B:
       control_lights(1, frame[1], frame[2], frame[3]);
       break;
    case 0xAA:
       digitalWrite(4,HIGH);
       lights_up(255,0,0);
       break;
    case 0xBB:
        digitalWrite(4,LOW);
        lights_up(0,0,0);
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
      break;
  }
  counter = 0;
}
