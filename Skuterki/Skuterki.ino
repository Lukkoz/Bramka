#define DISPALY_DEBUG
#include "display.h"
#include "gps.h"
#include "gsm.h"
#include <ArduinoJson.h>

const char scooterID[] = "47f1d6f6-4f9b-411a-964b-be7df46b7aa4";
const char NULLstr[] = "NULL";

const char message[] = "\n{\"email\":\"a@a.com\",\"password\":\"string\"}\n";
const char message_1[] = "\n{\"lat\":\"69\",\"long\":\"43\",\"soundActive\": true,\"battery\":{\"chargeLevel\": 100}}\n";
StaticJsonDocument<200> output;
bool isLocked = false;
bool soundActive = false;
bool openTrunk = false;
bool scooterPluged = false;

void setup(){
	#ifndef STATION
	Serial.begin(115200);
	init_display();
	print_on_display("READY DISP");
	init_gps();
	print_on_display("READY GPS");
	init_gsm();
	print_on_display("READY GSM");
	#else
	Serial.begin(115200);
	Serial.println("A");
	init_gsm();
	Serial.println("B");
	pinMode(14,INPUT_PULLUP);
	//runSerialPASS();
	#endif
}


void loop(){
	/*#ifndef DISPALY_DEBUG
	if(Serial.available() >0){
		char tmp = Serial.read();
		Serial.flush();
		if(tmp == 'I'){
			init_gsm();
		}else if(tmp == 'S'){
			runSerialPASS();
		}else if(tmp == 'T'){
			set_URL("http://time.jsontest.com");
			read_URL_json();
			runSerialPASS();
		}else if(tmp == 'L'){
			set_URL("https://api.sedaya.app/auth/login");
			post_data(message);
			print_buffer();
		}else if(tmp == 'P'){
			set_URL("https://api.sedaya.app/devices/scooters/1");
			post_data(message_1);
			parse_json_from_buffer(16);
		}else if(tmp =='G'){
			set_URL("https://reqres.in/api/products/1");
			read_URL_json();
			runSerialPASS();
		}
	}
	#else
	byte random_batt_state = millis()%100;
	read_gps_data();
	updateServer(1,current_gps_data.N,current_gps_data.E,random_batt_state,"false");
	#endif
	*/
#ifndef STATION
byte random_batt_state = millis()%100;
read_gps_data();

if(!soundActive && !openTrunk){
output = updateServerScooter(scooterID,current_gps_data.N,current_gps_data.E,random_batt_state);
}else{
output = updateServerScooter(scooterID,current_gps_data.N,current_gps_data.E,random_batt_state,"false","false");
}
  const char* id = output["id"];
  const char* user_cash = output["user"]["cashAmount"];
  isLocked = output["isLocked"];
  soundActive = output["soundActive"];
  openTrunk = output["openTrunk"];
  char tmp[50];
  if(output["user"]["id"] != nullptr){
  const char* user_id = output["user"]["id"];
  sprintf(tmp,"ID:%c%c%c\n$:%s\n|%d%d%d|",user_id[0],user_id[1],user_id[2],user_cash,isLocked,soundActive,openTrunk);
  }else{
  sprintf(tmp,"ID:NULL\n$:-\n|%d%d%d|",isLocked,soundActive,openTrunk);
  }
  print_on_display(tmp);
#else
if(digitalRead(14) == LOW && !scooterPluged){
	updateServerStation(scooterID);
	scooterPluged = true;
	Serial.println("Scooter plugged!");
}
if(digitalRead(14) == HIGH && scooterPluged){
	updateServerStation(NULLstr);
	scooterPluged = false;
	Serial.println("Scooter unplugged!");
}
#endif
}


void runSerialPASS(){
	Serial.println("Serial passthroug active");
	while(true){
		if(Serial.available()>0){
    		Serial2.write((char)Serial.read());
  		}
    	if(Serial2.available() >0){
    		Serial.print((char)Serial2.read());
  		}
	}
}