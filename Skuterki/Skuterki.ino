#include "display.h"
#include "gps.h"

void setup(){
	//init_dispaly();
	init_gps();
	Serial.begin(115200);

}

void loop(){
	delay(5000);
	read_gps_data();
	Serial.print(current_gps_data.N,6);
	Serial.print(":");
	Serial.println(current_gps_data.E,6);

}