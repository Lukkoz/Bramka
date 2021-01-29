#define DEBUG_MODE
#define MIN_REACTION_LEVEL 20
#define N_SAMPLE 15

#define R_BUTTON_PIN 34
#define G_BUTTON_PIN 32
#define B_BUTTON_PIN 35
#define INPUT_BUTTON_PIN 27
#define RS_MODE_PIN 21

#define BUTTON_PICK_TIME 5000
#define BUTTON_DEBOUNCE 50
#define AFTER_CLICK_DEBOUNCE 300
//BUTTON STATES:
#define IDLE 0
#define GAME_ONE 1
#define GAME_TWO 2

//PAD REQUESTS:
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
#define REACTION_CHANGE 13
#define WHITE 14
#define TRESHHOLD_SET 17
#define RAPORT_READOUT 18
#define READ_TEST 19
#define PADS_CONNECTED 12

byte padsConnected =PADS_CONNECTED;

byte pad_readouts[PADS_CONNECTED];
int pad_sums[PADS_CONNECTED] = {0};
byte buttonstate = 0;
long last_click;
bool game_active = false;
bool state_changed = false;
byte msg[5];
bool reaction = false;
bool panel_on = false;
byte pad_to_react = 0;
long on_time = 0;
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

void SetTreshold(byte addr,byte reaction_level){
  Serial2.write(addr);
  Serial2.write(TRESHHOLD_SET);
  Serial2.write(reaction_level);
  Serial2.write(addr^TRESHHOLD_SET^reaction_level);
}

void set_all(byte req){
  setPanel(0,req);
}

void setPanel(byte panelId,byte requestID){
  SendMessage(panelId,requestID);
}

void read_from_panel(byte panelId,byte command,byte nbytesToRead){
  SendMessage(panelId,command);
  enter_recive_mode();
  byte ii = 0;
  //Serial.println("Waiting for pad response");
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
}

void setup()
{
	pinMode(RS_MODE_PIN,OUTPUT);
  digitalWrite(RS_MODE_PIN,HIGH);
  Serial2.begin(250000);
  pinMode(R_BUTTON_PIN,OUTPUT);
  pinMode(G_BUTTON_PIN,OUTPUT);
  pinMode(B_BUTTON_PIN,OUTPUT);
  pinMode(INPUT_BUTTON_PIN,INPUT_PULLUP);
  digitalWrite(R_BUTTON_PIN,LOW);
  digitalWrite(G_BUTTON_PIN,LOW);
  digitalWrite(B_BUTTON_PIN,LOW);
  
  Serial.begin(250000);
  
 // enter_transmit_mode();
  #ifdef DEBUG_MODE
  Serial.println();
  Serial.println("config done");
  #endif
  delay(5000);
  #ifdef DEBUG_MODE
  Serial.println("Sending debug blink");
  #endif
  setPanel(0,RED);
  delay(1000);
  setPanel(0,OFF);
  delay(1000);
  //set_all(BLUE);
  
  /* Setting thresholds for panels in %
  SetTreshold(1,20);
  SetTreshold(2,20);
  SetTreshold(3,20);
  SetTreshold(4,20);
  SetTreshold(5,20);
  SetTreshold(6,20);
  SetTreshold(7,20);
  SetTreshold(8,20);
  SetTreshold(9,20);
  SetTreshold(10,20);
  SetTreshold(11,20);
  SetTreshold(12,20);
  */
  
}

byte raport_pad_status(bool debug_mode = false){
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

bool buttonCheck(){
  if(digitalRead(INPUT_BUTTON_PIN) == LOW){
    delay(BUTTON_DEBOUNCE);
    if(digitalRead(INPUT_BUTTON_PIN) == LOW){
      state_changed = true;
      last_click = millis();  
      return true;
    }
  }
  return false;
}

void start_game_one(){
  #ifdef DEBUG_MODE
  Serial.println("GAME ONE");
  #endif
  bool button_pressed = false;
  while(true){
  byte panel_to_hit = millis()%(padsConnected)+1;
  long game_started = millis();
  set_all(OFF);
  setPanel(panel_to_hit,BLUE);
  while(millis()- game_started < 20000){
      byte active_panel = raport_pad_status();
      if(active_panel !=0 && active_panel == panel_to_hit){
        setPanel(active_panel,GREEN);
        delay(2000);
        break;
      }else if(active_panel !=0 && active_panel != panel_to_hit){
        setPanel(active_panel,RED);
        delay(1000);
        setPanel(active_panel,OFF);
      }
    if(buttonCheck()){
      button_pressed = true;
      break;
    }
  }
  if(button_pressed)break;
  }
  #ifdef DEBUG_MODE
  Serial.println("GAME ENDED!");
  #endif
  set_all(OFF);
  game_active = false;
}

void start_game_two(){
  #ifdef DEBUG_MODE
  Serial.println("GAME TWO");
  #endif
  setPanel(0,OFF);
  while(true){
  set_all(BLUE);
  long game_started = millis();
  bool button_pressed = false;
  while(millis() - game_started < 45000){
    byte active_panel = raport_pad_status();
    if(active_panel != 0){
      setPanel(active_panel,GREEN);
    }
    if(buttonCheck()){
      button_pressed = true;
      break;
    }
  }
  if(button_pressed)break;
  }
  set_all(OFF);
  set_all(DISABLE_REACTION);
  set_all(REACTION_TIME_500);
  game_active = false;
}

void setButtonColor(byte state){
  switch(state){
  case 3:
   digitalWrite(R_BUTTON_PIN,LOW);
   digitalWrite(G_BUTTON_PIN,HIGH);
   digitalWrite(B_BUTTON_PIN,HIGH);
  break;
  case GAME_ONE:
   digitalWrite(R_BUTTON_PIN,HIGH);
   digitalWrite(G_BUTTON_PIN,LOW);
   digitalWrite(B_BUTTON_PIN,HIGH);
   set_all(GREEN);
  break;
  case GAME_TWO:
   digitalWrite(R_BUTTON_PIN,HIGH);
   digitalWrite(G_BUTTON_PIN,HIGH);
   digitalWrite(B_BUTTON_PIN,LOW);
   set_all(RED);
  break;
  case 4:
   digitalWrite(R_BUTTON_PIN,LOW);
   digitalWrite(G_BUTTON_PIN,LOW);
   digitalWrite(B_BUTTON_PIN,HIGH);
  break;
  case 5:
   digitalWrite(R_BUTTON_PIN,LOW);
   digitalWrite(G_BUTTON_PIN,HIGH);
   digitalWrite(B_BUTTON_PIN,LOW);
  break;
  case 6:
   digitalWrite(R_BUTTON_PIN,LOW);
   digitalWrite(G_BUTTON_PIN,LOW);
   digitalWrite(B_BUTTON_PIN,LOW);
  break;
  case IDLE:
   digitalWrite(R_BUTTON_PIN,HIGH);
   digitalWrite(G_BUTTON_PIN,HIGH);
   digitalWrite(B_BUTTON_PIN,HIGH);
   set_all(BLUE);
   //Serial2.println("IDLE");
  break;
  default:
  break;
  }
}
void idle(){
  switch (buttonstate){
    case IDLE:
     //raport_pad_status(true);
    break;
    case GAME_ONE:
      if(!game_active && (millis() - last_click) > BUTTON_PICK_TIME){
        start_game_one();
        game_active = false;
      }
    break;
    case GAME_TWO:
      if(!game_active && (millis() - last_click) > BUTTON_PICK_TIME){
        start_game_two();
        game_active = false;
      }
    break;
  }
}

void loop()
{
  if(buttonCheck()){
    buttonstate++;
    if(buttonstate == 4){
      buttonstate = 0;
    }
    setButtonColor(buttonstate);
    game_active = false;
    delay(AFTER_CLICK_DEBOUNCE);
  }
  idle();
  
  }





