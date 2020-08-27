#define R_BUTTON_PIN 34
#define G_BUTTON_PIN 32
#define B_BUTTON_PIN 35
#define INPUT_BUTTON_PIN 27
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

byte padsConnected =12;

byte buttonstate = 0;
long last_click;
bool game_active = false;
bool state_changed = false;



void SendMessage(byte addr,byte command){
  Serial.write(addr);
  delay(1);
  Serial.write(command);
  delay(1);
  Serial.write(addr^command);
  delay(30);
}

void SetTreshold(byte addr,byte reaction_level){
  Serial.write(addr);
  delay(1);
  Serial.write(TRESHHOLD_SET);
  delay(1);
  Serial.write(reaction_level);
  delay(1);
  Serial.write(addr&^TRESHHOLD_SET^reaction_level);
  delay(30);
}

void set_all(byte req){
  setPanel(0,req);
}

void setPanel(byte panelId,byte requestID){
  SendMessage(panelId,requestID);
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
  Serial.begin(9600);
  pinMode(21,OUTPUT);
  digitalWrite(21,HIGH);
  delay(8000);
  //set_all(15);
  delay(1000);
  set_all(BLUE);
  delay(1000);
  
  // Setting thresholds for panels in %
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
  //

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
  //Serial.println("GAME ONE");
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
  //Serial.println("GAME ENDED!");
  set_all(OFF);
  set_all(DISABLE_REACTION);
  game_active = false;
}

void start_game_two(){
  //Serial.println("GAME TWO");
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
  //Serial.println("GAME ENDED!");
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
   //Serial.println("IDLE");
  break;
  default:
  break;
  }
}
void idle(){
  switch (buttonstate){
    case IDLE:
  
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






