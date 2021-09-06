//#define DISPALY_DEBUG
#include "display.h"
#include "gps.h"
#include "gsm.h"
#include <ArduinoJson.h>

const char scooterID[] = "47f1d6f6-4f9b-411a-964b-be7df46b7aa4";
const char NULLstr[] = "NULL";

StaticJsonDocument<200> output;
bool isLocked = false;
bool soundActive = false;
bool openTrunk = false;
bool scooterPluged = false;
bool isCharging = false;

void setup(){
	#ifndef STATION
	Serial.begin(115200);
	init_gsm();
	init_gps();
	pinMode(14,INPUT_PULLUP);
	#else
	Serial.begin(115200);
	init_gsm();
	init_gps();
	#endif
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