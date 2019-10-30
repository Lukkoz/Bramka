#include "Skribot.h"
#define LED_INT 150
#define S1 0
#define S2 2
#define S3 15
#define S0 4

byte padsConnected =2;

SPIHandler *comm;
byte output_buffer[16] = {0};

byte Header_byte(byte nSend,byte nRec){
  return(nSend | (nRec<<4));             //number of bytes to send        
}

void SendMessage(byte addr,byte command, byte R = 0,byte G = 0, byte B = 0){
  byte msg[] = {Header_byte(4,0),command,R,G,B};
  SetMultiOUT(addr);
  comm->SPITransfer(msg);
  ClearMulti();
}

void SetMultiOUT(byte out){
  if(out & 1 != 0)digitalWrite(S0,HIGH);
  if(out & 1<<1 != 0)digitalWrite(S1,HIGH);
  if(out & 1<<2 != 0)digitalWrite(S2,HIGH);
  if(out & 1<<3 != 0)digitalWrite(S3,HIGH);
}
void ClearMulti(){
  digitalWrite(S1,LOW);
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  digitalWrite(S0,LOW);
}
void setup()
{
	Serial.begin(115200);
	comm = new SPIHandler(13,12,14,22);
	comm->set_SPI_Settings(4000000, MSBFIRST, SPI_MODE0);
  comm->set_SPI_bit_format(8);
  
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(S0,OUTPUT);
  ClearMulti();

}

void loop()
{
	String input = " ";
  if(Serial.available()){
    input = Serial.readString();
    if(input == "ON"){
      for(byte tt = 0; tt<padsConnected;tt++){
        SendMessage(tt,0x0C,0,LED_INT,0); 
      }
    }
    else if(input == "OFF"){
      for(byte tt = 0; tt<padsConnected;tt++){
        SendMessage(tt,0x0B); 
      }
  }else if(input == "1"){
    SendMessage(1,0xAD,LED_INT,0,0);
    SendMessage(2,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if(input == "2"){
    SendMessage(2,0xAD,LED_INT,0,0);
    SendMessage(1,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if(input == "3"){
    SendMessage(1,0xAD,0,LED_INT,0);
    SendMessage(2,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if("TEST"){
      SendMessage(1,0xAA,0,LED_INT,0);
      SendMessage(2,0xAA,0,LED_INT,0);
      delay(500);
      SendMessage(1,0xBB,0,LED_INT,0);
      SendMessage(2,0xBB,0,LED_INT,0);
  }

}
}




