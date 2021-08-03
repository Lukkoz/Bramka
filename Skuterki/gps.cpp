#include "gps.h"

TinyGPSPlus gps;
gps_data current_gps_data;


void init_gps(){
	SERIAL_PORT_GPS.begin(9600, SERIAL_8N1, 25,26);//4, 5);
}
gps_data read_gps_data(){
	SERIAL_PORT_GPS.flush();
	delay(1000);
	check_for_gps_data();
	return(current_gps_data);
}

void check_for_gps_data() {
  Serial.println("GPS DATA:");
while (SERIAL_PORT_GPS.available() > 0){
    char tmp = SERIAL_PORT_GPS.read();
    Serial.print(tmp);
    if (gps.encode(tmp)){
      current_gps_data.N = gps.location.lat();
      current_gps_data.E = gps.location.lng();
      current_gps_data.hour = gps.time.hour();
      current_gps_data.minute = gps.time.minute();
     
    }
  }

}