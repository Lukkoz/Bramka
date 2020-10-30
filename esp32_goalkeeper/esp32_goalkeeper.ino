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

byte padsConnected =12;

byte buttonstate = 0;
long last_click;
bool game_active = false;
bool state_changed = false;
byte msg[5];

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
  Serial2.write(addr^command);
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
  Serial.println("waiting for msg");
  while(nbytesToRead !=0){
    if(Serial2.available() > 0){
      msg[ii] = Serial2.read();
      Serial.println(msg[ii]);
      ii++;
      nbytesToRead--;
    }
  }
  //delay(1000);
  enter_transmit_mode();
}

void setup()
{
	pinMode(R_BUTTON_PIN,OUTPUT);
  pinMode(G_BUTTON_PIN,OUTPUT);
  pinMode(B_BUTTON_PIN,OUTPUT);
  pinMode(INPUT_BUTTON_PIN,INPUT_PULLUP);
  digitalWrite(R_BUTTON_PIN,LOW);
  digitalWrite(G_BUTTON_PIN,LOW);
  digitalWrite(B_BUTTON_PIN,LOW);
  Serial.begin(250000);
  Serial2.begin(250000);
  pinMode(RS_MODE_PIN,OUTPUT);
  enter_transmit_mode();
  Serial.println();
  Serial.println("config done");
  delay(1000);
  /*setPanel(12,RED);
  delay(1000);
  setPanel(12,OFF);
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
  Serial.println("Test readout form panel:");
  read_from_panel(12,READ_TEST,1);
  Serial.print("Message:");
  Serial.println(msg[0]);
  
}

void raport_pad_status(){
  for(int ii = 1; ii< padsConnected+1;ii++){
    read_from_panel(ii,RAPORT_READOUT,1);
    Serial.print(ii);
    Serial.print(":");
    Serial.print(msg[0]);
    if(ii%4==0)Serial.println();
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
  //Serial2.println("GAME ONE");
  while(true){
 set_all(OFF);
 set_all(REACTION_RED);
  byte panel_to_hit = millis()%(padsConnected)+1;
  setPanel(panel_to_hit,REACTION_GREEN);
  setPanel(panel_to_hit,BLUE);
  long game_started = millis();
  bool button_pressed = false;
  while(millis()- game_started < 20000){
    if(buttonCheck()){
      button_pressed = true;
      break;
    }
  }
  if(button_pressed)break;
  }
  //Serial2.println("GAME ENDED!");
  set_all(OFF);
  set_all(DISABLE_REACTION);
  game_active = false;
}

void start_game_two(){
  //Serial2.println("GAME TWO");
  setPanel(0,OFF);
  while(true){
  set_all(BLUE);
  set_all(REACTION_GREEN);
  set_all(REACTION_CHANGE);
  long game_started = millis();
  bool button_pressed = false;
  while(millis() - game_started < 45000){
    if(buttonCheck()){
      button_pressed = true;
      break;
    }
  }
  if(button_pressed)break;
  }
  //Serial2.println("GAME ENDED!");
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
      //raport_pad_status();
      setPanel(12,RED);
      setPanel(12,OFF);
      Serial.println("Test readout form panel:");
      read_from_panel(12,READ_TEST,1);
      Serial.print("Message:");
      Serial.println(msg[0]);
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
    if(buttonstate == 3){
      buttonstate = 0;
    }
    setButtonColor(buttonstate);
    game_active = false;
    delay(AFTER_CLICK_DEBOUNCE);
  }
  idle();
}





