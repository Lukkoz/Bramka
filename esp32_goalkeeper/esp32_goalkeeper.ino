#include "Skribot.h"
#define LED_INT 150
#define S0 18
#define S1 5
#define S2 17
#define S3 16
#define Z  25
#define EN 26

//Colors:
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


byte padsConnected =2;

SPIHandler *mux;
byte output_buffer[16] = {0};

byte Header_byte(byte nSend,byte nRec){
  return(nSend | (nRec<<4));             //number of bytes to send        
}

void SendMessage(byte addr,byte command){
  SetCS(addr);
  delay(5);
  Serial.write(command);
  ClearCS(addr);
}

void SetMUXIN(byte out){
  if(out & 1 != 0)digitalWrite(S0,HIGH);
  if(out & 1<<1 != 0)digitalWrite(S1,HIGH);
  if(out & 1<<2 != 0)digitalWrite(S2,HIGH);
  if(out & 1<<3 != 0)digitalWrite(S3,HIGH);
}
void ClearCS(byte out){
  byte msg[] = {Header_byte(2,0),0x0B,out};
  mux->SPITransfer(msg);
}
void SetCS(byte out){
  byte msg[] = {Header_byte(2,0),0x0C,out};
  mux->SPITransfer(msg);
}

void ClearMulti(){
  digitalWrite(S1,LOW);
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  digitalWrite(S0,LOW);
}

void setPanelColor(byte panelId,byte colorID){
  SendMessage(panelId,colorID);
}

void setPanelReactionColor(byte panelId,byte colorID){
  SendMessage(panelId,colorID);
}

void disableReaction(byte panelId){
  SendMessage(panelId,DISABLE_REACTION);
}

void setReactionTime(byte panelId,byte reactionSeconds){
  SendMessage(panelId,reactionSeconds);
}

bool CheckPromptPin(byte panelId){
  SetMUXIN(panelId);
  return(digitalRead(Z) == HIGH);
}

void setup()
{
	Serial.begin(9600);

  mux = new SPIHandler(0,2,15,4);
  mux->set_SPI_Settings(4000000, MSBFIRST, SPI_MODE0);
  mux->set_SPI_bit_format(8);
  
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(S0,OUTPUT);
  ClearMulti();

}

void loop()
{
  setPanelColor(1,RED);
  setPanelColor(2,OFF);
  delay(500);
  setPanelColor(2,RED);
  setPanelColor(1,OFF);
  delay(500);
	/*String input = " ";
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
    setPanelReactionColor(1,0,LED_INT,0);
    setPanelReactionColor(2,LED_INT,0,0);
    setPanelColor(1,255,191,0);
    setPanelColor(2,0,0,0);
    Serial.println("OK");
  }else if(input == "2"){
    setPanelReactionColor(2,LED_INT,0,0);
    setPanelReactionColor(1,0,LED_INT,0);
    setPanelColor(2,255,191,0);
    setPanelColor(2,0,0,0);
    Serial.println("OK");
  }else if(input == "3"){
    SendMessage(1,0xAD,0,LED_INT,0);
    SendMessage(2,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if(input == "TEST"){
      setPanelColor(1,0,0,255);
  }

}*/
}





