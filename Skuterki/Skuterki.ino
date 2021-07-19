//#define DISPALY_DEBUG
#include "display.h"
#include "gps.h"
#include "gsm.h"

const char message[] = "\n{\"email\":\"a@a.com\",\"password\":\"string\"}\n";
const char message_1[] = "\n{\"lat\":\"69\",\"long\":\"43\",\"soundActive\": true,\"battery\":{\"chargeLevel\": 100}}\n";

void setup(){
	#ifndef DISPALY_DEBUG
	Serial.begin(9600);
	init_gsm();
	init_gps();
	#else
	init_display();
	print_on_display("READY DISP");
	init_gps();
	print_on_display("READY GPS");
	init_gsm();
	print_on_display("READY GSM");
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
	byte random_batt_state = millis()%100;
	read_gps_data();
	updateServer(1,current_gps_data.N,current_gps_data.E,random_batt_state,"false");
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