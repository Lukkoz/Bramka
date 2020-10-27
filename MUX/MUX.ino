#include <SPI.h>
volatile boolean received;
volatile byte Slavereceived,Slavesend;
#define PAD_1 9 
#define PAD_2 8 
#define PAD_3 7 
#define PAD_4 6 
#define PAD_5 14 
#define PAD_6 15
#define PAD_7 16 
#define PAD_8 17
#define PAD_9 18
#define PAD_10 19 
#define PAD_11 A6 
#define PAD_12 A7 
char out_buffer[4];
byte frame[6];
int counter = 0;

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
    if(counter == 2)handle_message(frame);
}

void setOutConfig(byte out,byte state){
    switch(out){
            case 1:
        digitalWrite(PAD_1,state);
      break;
            case 2:
        digitalWrite(PAD_2,state);
      break;
            case 3:
        digitalWrite(PAD_3,state);
      break;
            case 4:
        digitalWrite(PAD_4,state);
      break;
            case 5:
        digitalWrite(PAD_5,state);
      break;
            case 6:
        digitalWrite(PAD_6,state);
      break;
            case 7:
        digitalWrite(PAD_7,state);
      break;
            case 8:
        digitalWrite(PAD_8,state);
      break;
            case 9:
        digitalWrite(PAD_9,state);
      break;
            case 10:
        digitalWrite(PAD_10,state);
      break;
            case 11:
        digitalWrite(PAD_11,state);
      break;
            case 12:
        digitalWrite(PAD_12,state);
      break;
            default:
      break;

    }
}

void setup()

{
  Serial.begin(115200);
  
 

  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  SPCR |= _BV(SPIE);
  received = false;
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder (MSBFIRST);
  pinMode(PAD_1,OUTPUT);
  pinMode(PAD_2,OUTPUT);
  pinMode(PAD_3,OUTPUT);
  pinMode(PAD_4,OUTPUT);
  pinMode(PAD_5,OUTPUT);
  pinMode(PAD_6,OUTPUT);
  pinMode(PAD_7,OUTPUT);
  pinMode(PAD_8,OUTPUT);
  pinMode(PAD_9,OUTPUT);
  pinMode(PAD_10,OUTPUT);
  pinMode(PAD_11,OUTPUT);
  pinMode(PAD_12,OUTPUT);

  digitalWrite(PAD_1,HIGH);
  digitalWrite(PAD_2,HIGH);
  digitalWrite(PAD_3,HIGH);
  digitalWrite(PAD_4,HIGH);
  digitalWrite(PAD_5,HIGH);
  digitalWrite(PAD_6,HIGH);
  digitalWrite(PAD_7,HIGH);
  digitalWrite(PAD_8,HIGH);
  digitalWrite(PAD_9,HIGH);
  digitalWrite(PAD_10,HIGH);
  digitalWrite(PAD_11,HIGH);
  digitalWrite(PAD_12,HIGH);

}


void loop(){


}

void handle_message(byte frame[5]) {
  byte checksum = 0;
  switch (frame[0]) {
      break;
    case 0x0C:
       setOutConfig(frame[1],LOW);
      break;
    case 0x0B:
        setOutConfig(frame[1],HIGH);
       break;
    default: 
      break;
  }
  counter = 0;
}
