#include "pad_operations.h"

byte padsConnected =PADS_CONNECTED;
byte pad_readouts[PADS_CONNECTED];
int pad_sums[PADS_CONNECTED] = {0};
byte msg[5];
byte pad_to_react = 0;
byte pad_history[30][12];
bool hit_event = false;
byte event_iterator = 0;
bool valid_message = true;

void enter_transmit_mode(){
  digitalWrite(RS_MODE_PIN,HIGH);
}

void enter_recive_mode(){
  Serial2.flush();
  digitalWrite(RS_MODE_PIN,LOW);
}

void SendMessage(byte addr,byte command){
  Serial2.write(addr);
  Serial2.write(command);
  byte tmp = addr^command;
  tmp+=4;
  Serial2.write(tmp);
}

void set_all(byte req){
  setPanel(0,req);
}

void setPanel(byte panelId,byte requestID){
  SendMessage(panelId,requestID);
}

byte read_from_panel(byte panelId,byte command,byte nbytesToRead){
  SendMessage(panelId,command);
  enter_recive_mode();
  byte ii = 0;
  long resptime = millis();
  while(nbytesToRead !=0){
    if((millis() - resptime) > 50){
      #ifdef DEBUG_MODE
      Serial.print("Timmout at slave:");
      Serial.println(panelId);
      #endif
      for(byte uu = 0; uu<ii;uu++){
        Serial.println(msg[uu]);
      }
      for(byte tt = 0;tt<nbytesToRead;tt++)msg[tt] = 0;
      break;
    }
    if(Serial2.available() > 0){
      msg[ii] = Serial2.read();
      ii++;
      nbytesToRead--;
    }
    if(nbytesToRead == 0){
      if(msg[0] == 13 && (msg[0]^msg[1])+4 == msg[2]){
        valid_message = true;
      }else{
        valid_message = false;
      }
    }
  }
  enter_transmit_mode();
  return(msg[1]);
}

byte raport_pad_status(bool debug_mode){
  bool nonZero = false;
  for(int ii = 1; ii< padsConnected+1;ii++){
    read_from_panel(ii,RAPORT_READOUT,3);
    pad_readouts[ii-1] = msg[1];
    if(pad_readouts[ii-1] > MIN_REACTION_LEVEL)hit_event = true;
  }
  if(hit_event){
     int max_readout = MIN_REACTION_LEVEL;
    for(int jj = 0; jj< padsConnected;jj++){ 
        pad_history[event_iterator][jj] = pad_readouts[jj];
  
      }
      event_iterator++;
    if(event_iterator == N_SAMPLE){
      hit_event = false;
      event_iterator = 0;
      #ifdef DEBUG_MODE
      Serial.println("##############################################");
      #endif
      int max_readout = MIN_REACTION_LEVEL;
      for(byte rr = 0; rr<padsConnected; rr++){
      #ifdef DEBUG_MODE
        Serial.print("Pad");
        Serial.print(rr+1);
        Serial.print(": ");
      #endif
        for(byte hh = 0; hh<N_SAMPLE;hh++){
          #ifdef DEBUG_MODE
          Serial.print(pad_history[hh][rr]);
          Serial.print("  ");
          #endif
          if(pad_history[hh][rr]>MIN_REACTION_LEVEL)pad_sums[rr]+=pad_history[hh][rr];
        }
       
      if(pad_sums[rr] > max_readout){
          max_readout = pad_sums[rr];
          pad_to_react = rr+1;
       }
       #ifdef DEBUG_MODE
        Serial.print("|");
        Serial.print(pad_sums[rr]);
        Serial.print("|");
        Serial.println();
       #endif
      }
      if(debug_mode){
      for(byte kk = 0;kk < PADS_CONNECTED; kk++){
      if(pad_sums[kk] == max_readout)
      {
        setPanel(kk+1,RED);
      }else if(pad_sums[kk] > max_readout*3/4){
        setPanel(kk+1,BLUE);
      }else if(pad_sums[kk] > max_readout/2){
        setPanel(kk+1,GREEN);
      }
      }
      delay(2000);
      setPanel(0,OFF);
      delay(500);
      }
      for(byte kk = 0;kk < PADS_CONNECTED; kk++)pad_sums[kk] =0;
      return(pad_to_react);
    }
    }else{
      return(0);
    }
  
  
}