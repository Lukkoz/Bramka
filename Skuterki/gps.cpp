#include "gps.h"

TinyGPSPlus gps;
gps_data current_gps_data;

void init_gps(){
	Serial2.begin(9600);
}
gps_data read_gps_data(){
	Serial2.flush();
	delay(1000);
	check_for_gps_data();
	return(current_gps_data);
}

void check_for_gps_data() {
while (Serial2.available() > 0){
    char tmp = Serial2.read();
    //Serial.print(tmp);
    if (gps.encode(tmp)){
      current_gps_data.N = gps.location.lat();
      current_gps_data.E = gps.location.lng();
      current_gps_data.hour = gps.time.hour();
      current_gps_data.minute = gps.time.minute();
    }

  }
}