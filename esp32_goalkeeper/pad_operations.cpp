#include "pad_operations.h"

byte padsConnected =PADS_CONNECTED;
int pad_readouts[PADS_CONNECTED];
int pad_sums[PADS_CONNECTED] = {0};
int pad_d_sums[PADS_CONNECTED] = {0};
byte msg[5];
byte pad_to_react = 0;
byte pad_history[N_SAMPLE][PADS_CONNECTED];
bool hit_event = false;
byte event_iterator = 0;
bool valid_message = true;
int max_readout = MIN_REACTION_LEVEL;

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

int read_from_panel(byte panelId,byte command,byte nbytesToRead){
  SendMessage(panelId,command);
  enter_recive_mode();
  byte ii = 0;
  long resptime = millis();
  while(nbytesToRead !=0){
    if((millis() - resptime) > 10){
      #ifdef DEBUG_MODE
      //Serial.print("Timmout at slave:");
      //Serial.println(panelId);
      #endif
      enter_transmit_mode();
      return(-1);
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

void cooldown_after_hit(){
  bool tmp = true;
  byte n = 0;
  long cooldown_time_start = millis();
  while(true){
  tmp =  false;
  for(int ii = 1; ii< padsConnected+1;ii++){
    pad_readouts[ii-1] = read_from_panel(ii,RAPORT_READOUT,3);
    if(pad_readouts[ii-1] < 0 || pad_readouts[ii-1] > COOLDOWN_LEVEL) tmp = true;
    if(ii == pad_to_react && pad_readouts[ii-1] > 0)Serial.println(pad_readouts[ii-1]);
    }
    if(!tmp){
      n++;
    }else{
      n =0;
    }
    if(millis() - cooldown_time_start > 20000)break;
    if(n >2)break;
  }
}

void raport_pads(bool hit_only){
      if(hit_only){
      for(byte hh = 0; hh<N_SAMPLE;hh++){
          if(pad_history[hh][pad_to_react-1] > 0)Serial.println(pad_history[hh][pad_to_react-1]);
        }
      }else{
      for(byte rr = 0; rr<padsConnected; rr++){
        Serial.print("P");
        Serial.print(rr+1);
        Serial.print("<-c(");
        
        for(byte hh = 1; hh<N_SAMPLE;hh++){
          Serial.print(pad_history[hh][rr]);
          if(hh != N_SAMPLE-1)Serial.print(",");
          pad_d_sums[rr] +=(abs(pad_history[hh][rr]-pad_history[hh-1][rr]));
        }
        /*
        Serial.print("|");
        Serial.print(pad_d_sums[rr]);
        Serial.print("|");
        */
        Serial.println(")");
        pad_d_sums[rr] = 0;
      }
      Serial.print("plot(P");
      Serial.print(pad_to_react);
      Serial.println(")");
    }
}

void get_event_data(){
  for(byte tt =  0; tt<N_SAMPLE;tt++){
    for(int ii = 1; ii< padsConnected+1;ii++){
      pad_history[event_iterator][ii-1] = read_from_panel(ii,RAPORT_READOUT,3);
    }
      event_iterator++;
    }
    hit_event = false;
    event_iterator = 0;
    for(byte rr = 0; rr<padsConnected; rr++){
      for(byte hh = 0; hh<N_SAMPLE;hh++){
        if(pad_history[hh][rr]>MIN_REACTION_LEVEL)pad_sums[rr]+=pad_history[hh][rr];
      }
        if(pad_sums[rr] > max_readout){
          max_readout = pad_sums[rr];
          pad_to_react = rr+1;
       }
     }
}

void check_for_hit(){
    for(int ii = 1; ii< padsConnected+1;ii++){
    pad_readouts[ii-1] = read_from_panel(ii,RAPORT_READOUT,3);
    if(pad_readouts[ii-1] > MIN_REACTION_LEVEL){
      hit_event = true;
      break;
    }
  }
}

byte raport_pad_status(bool debug_mode){
  check_for_hit();
  if(hit_event){
    get_event_data();
    raport_pads();
      if(debug_mode){
      bool main_pad_reacted = false;
      for(byte kk = 0;kk < PADS_CONNECTED; kk++){
      if(!main_pad_reacted && pad_sums[kk] == max_readout)
      {
        setPanel(kk+1,RED);
        main_pad_reacted = true;
      }else if(pad_sums[kk] > max_readout*3/4){
        setPanel(kk+1,BLUE);
      }else if(pad_sums[kk] > max_readout/2){
        setPanel(kk+1,GREEN);
      }
      }
      delay(500);
      cooldown_after_hit();
      setPanel(0,OFF);
      //delay(50);
      }
      for(byte kk = 0;kk < PADS_CONNECTED; kk++)pad_sums[kk] =0;
      max_readout = MIN_REACTION_LEVEL;
      return(pad_to_react);
    }
    return(0);  
}