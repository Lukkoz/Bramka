#define R_BUTTON_PIN 34
#define G_BUTTON_PIN 32
#define B_BUTTON_PIN 35
#define INPUT_BUTTON_PIN 33

#define BUTTON_PICK_TIME 5000
#define BUTTON_DEBOUNCE 50
#define AFTER_CLICK_DEBOUNCE 300
//STATES:
#define IDLE 0
#define GAME_ONE 1
#define GAME_TWO 2
#define WIFI_MODE 3

#include "goalkeeper_wifi.h"
#include "pad_operations.h"

byte buttonstate = 0;
byte user_state = 0;
long last_click;
bool game_active = false;
bool state_changed = false;
bool reaction = false;
bool panel_on = false;
long on_time = 0;
bool wifi_control = false;


void setup()
{

  pinMode(R_BUTTON_PIN,OUTPUT);
  pinMode(G_BUTTON_PIN,OUTPUT);
  pinMode(B_BUTTON_PIN,OUTPUT);
  pinMode(INPUT_BUTTON_PIN,INPUT_PULLUP);
  digitalWrite(R_BUTTON_PIN,LOW);
  digitalWrite(G_BUTTON_PIN,HIGH);
  digitalWrite(B_BUTTON_PIN,LOW); 
  pinMode(RS_MODE_PIN,OUTPUT);
  digitalWrite(RS_MODE_PIN,HIGH);
  delay(2000);
  Serial.begin(250000);
  Serial2.begin(250000);
  Serial2.flush();
  #ifdef DEBUG_MODE
  Serial.println();
  Serial.println("config done");
  #endif
  #ifdef DEBUG_MODE
  Serial.println("Sending debug blink");
  #endif
  setPanel(0,RED);
  delay(1000);
  setPanel(0,OFF);
  delay(1000);
 /*while(true){
 int tmp =read_from_panel(11,RAPORT_READOUT,3);
  Serial.println(tmp);
  }
  */
  
}

bool user_action_check(){
  byte tmp_state = user_state;
  if(wifi_control){
    user_state = wifi_check_for_client();
    buttonCheck();
    if(state_changed){
      wifi_control = false;
      wifi_end();
      user_state = IDLE;
      state_changed = false;
    }
  }else{
    user_state = buttonCheck();
    state_changed = false;
  }
  if(tmp_state != user_state){
    game_active = false;
    setButtonColor(user_state);
    delay(AFTER_CLICK_DEBOUNCE);
    return(true);
  }
  return(false);
}

byte buttonCheck(){
  if(digitalRead(INPUT_BUTTON_PIN) == LOW){
    delay(BUTTON_DEBOUNCE);
    if(digitalRead(INPUT_BUTTON_PIN) == LOW){
      state_changed = true;
      last_click = millis();  
      buttonstate++;
      if(buttonstate == 4){
        buttonstate = 0;
      }
    }
  }
  return(buttonstate);
}

void start_game_one(){
  #ifdef DEBUG_MODE
  Serial.println("GAME ONE");
  #endif
  bool user_interrupt = false;
  while(true){
  byte panel_to_hit = millis()%(PADS_CONNECTED)+1;
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
      if(active_panel != 0)cooldown_after_hit();
    if(user_action_check()){
      user_interrupt = true;
      break;
    }
  }
  if(user_interrupt)break;
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
  while(true){
  set_all(OFF);
  long game_started = millis();
  bool user_interrupt = false;
  set_all(BLUE);
  delay(200);
  while(millis() - game_started < 60000){
    byte active_panel = raport_pad_status();
    if(active_panel != 0){
      setPanel(active_panel,GREEN);
      cooldown_after_hit();
    }
    if(user_action_check()){
      user_interrupt = true;
      break;
    }
  }
  if(user_interrupt)break;
  }
  set_all(OFF);
  game_active = false;
}

void setButtonColor(byte state){
  switch(state){
  case GAME_ONE:
   set_all(GREEN);
  break;
  case GAME_TWO:
   set_all(RED);
  break;
  case WIFI_MODE:
   set_all(WHITE);
  break;
  case IDLE:
   set_all(OFF);
  break;
  default:
  break;
  }
}
void idle(){
  switch (user_state){
    case IDLE:
        raport_pad_status(true);
    break;
    case GAME_ONE:
      if(wifi_control || (!game_active && (millis() - last_click) > BUTTON_PICK_TIME)){
        start_game_one();
        game_active = false;
      }
    break;
    case GAME_TWO:
      if(wifi_control || (!game_active && (millis() - last_click) > BUTTON_PICK_TIME)){
        start_game_two();
        game_active = false;
      }
    break;
    case WIFI_MODE:
      if(!wifi_control){
        if((millis() - last_click) > BUTTON_PICK_TIME){
          wifi_control = true;
          wifi_begin();
          user_state = IDLE;
          setButtonColor(user_state);
        }
      }

    break;
  }
}

void loop()
{

  user_action_check();    
  
  idle();
}





