#include <SPI.h>
volatile boolean received;
volatile byte Slavereceived,Slavesend;

void setup()

{
  Serial.begin(115200);
  
  pinMode(4,OUTPUT);                
  digitalWrite(4,HIGH);
  delay(500);
  digitalWrite(4,LOW);
  delay(500);
  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 

  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  SPCR |= _BV(SPIE);
  received = false;
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder (MSBFIRST);
}

ISR (SPI_STC_vect)                        //Inerrrput routine function 
{
  Slavereceived = SPDR;         // Value received from master if store in variable slavereceived
  received = true;  
  //Sets received as True 
}

void loop(){
  if(received){
    switch(Slavereceived){
      case 1:
        digitalWrite(4,LOW);
      break;
      case 2:
        digitalWrite(4,HIGH);
      break;
      default:
      break;
    }
    received = false;
  }
   
      

}
