//#define DISPALY_DEBUG
#include "display.h"
#include "gps.h"
#include "gsm.h"
#include <ArduinoJson.h>

#define LOCKED_SWITCH 33
#define NOISE_SWITCH 25
#define TRUNK_SWITCH 32

const char scooterID[] = "47f1d6f6-4f9b-411a-964b-be7df46b7aa4";
const char NULLstr[] = "NULL";

StaticJsonDocument<200> output;
bool isLocked = true;
bool soundActive = false;
bool openTrunk = false;
bool scooterPluged = false;
bool isCharging = false;

void setup(){
  pinMode(LOCKED_SWITCH,OUTPUT);
  pinMode(NOISE_SWITCH,OUTPUT);
  pinMode(TRUNK_SWITCH,OUTPUT);
  digitalWrite(LOCKED_SWITCH,HIGH);
  digitalWrite(NOISE_SWITCH,HIGH);
  digitalWrite(TRUNK_SWITCH,HIGH);
  pinMode(GPS_STATUS_PIN,OUTPUT);
  digitalWrite(GPS_STATUS_PIN,LOW);
  for(byte tt =0; tt < 20;tt++){
    digitalWrite(GPS_STATUS_PIN, HIGH);
    delay(500);
    digitalWrite(GPS_STATUS_PIN,LOW);
    delay(500);

  }
	#ifndef STATION
	Serial.begin(115200);
	init_gsm();
  init_gps();
	#else
	Serial.begin(115200);
	init_gsm();
  init_gps();
	#endif
  pinMode(14,INPUT_PULLUP);

}


void loop(){
#ifndef STATION
delay(2000);
byte random_batt_state = millis()%100;
read_gps_data();
if(!soundActive && !openTrunk){
output = updateServerScooter(scooterID,current_gps_data.N,current_gps_data.E,random_batt_state);
}else{
output = updateServerScooter(scooterID,current_gps_data.N,current_gps_data.E,random_batt_state,"false","false","false");
}
  const char* id = output["id"];
  const char* user_cash = output["user"]["cashAmount"];
  isLocked = output["isLocked"];
  soundActive = output["soundActive"];
  openTrunk = output["openTrunk"];
  Serial.print("IsLocked:");
  Serial.println(isLocked);
  Serial.print("soundActive:");
  Serial.println(soundActive);
  Serial.print("openTrunk:");
  Serial.println(openTrunk);
  Serial.print("Active user:");
  if(output["user"]["id"] != nullptr){
  	const char* user_id = output["user"]["id"];
  	Serial.println(user_id);
  	Serial.print("User cash:");
  	Serial.println(user_cash);
  }else{
  	Serial.println("None");
  } 
  if(isLocked){
    digitalWrite(LOCKED_SWITCH,HIGH);
  }else{
    digitalWrite(LOCKED_SWITCH,LOW);
  }
  if(soundActive){
    digitalWrite(NOISE_SWITCH,LOW);
    delay(2000);
    digitalWrite(NOISE_SWITCH,HIGH);
  }
  if(openTrunk){
    digitalWrite(TRUNK_SWITCH,LOW);
    delay(2000);
    digitalWrite(TRUNK_SWITCH,HIGH);
  }
#else

	updateServerStation(scooterID);
	scooterPluged = true;
	Serial.println("Scooter plugged!");
	delay(5000);

	updateServerStation(NULLstr);
	scooterPluged = false;
	Serial.println("Scooter unplugged!");
	delay(5000);
#endif
}


void runSerialPASS(){
	Serial.println("Serial passthroug active");
	while(true){
		if(Serial.available()>0){
    		char tmp = Serial.read();
    		if(tmp!='^'){
    			Serial2.write(tmp);
    		}else{
    			Serial2.write(26);
    		}
  		}
    	if(Serial2.available() >0){
    		Serial.print((char)Serial2.read());
  		}
	}
}