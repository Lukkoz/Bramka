#include "gps.h"

gps_data current_gps_data;


void init_gps(){
	send_cmd("AT+CVAUXV=3050");
  send_cmd("AT+CVAUXS=1");
  send_cmd("AT+CGPSHOR=50");
  send_cmd("AT+CGPSAUTO=1");
}
bool read_gps_data(){
	send_cmd("AT+CGPSINFO",3,1);
  response gps_data_string = get_value_after("+CGPSINFO: ");
  bool tmp = true;
  if(gps_data_string.len>50){
    current_gps_data.N = cti(gps_data_string.value[0])*10+cti(gps_data_string.value[1])
                          +(float)(10*cti(gps_data_string.value[2])+cti(gps_data_string.value[3]))/60
                          +(float)cti(gps_data_string.value[5])/600
                          +(float)cti(gps_data_string.value[6])/6000
                          +(float)cti(gps_data_string.value[7])/60000
                          +(float)cti(gps_data_string.value[8])/600000
                          +(float)cti(gps_data_string.value[9])/6000000
                          +(float)cti(gps_data_string.value[10])/60000000;
    current_gps_data.E = cti(gps_data_string.value[14])*100+cti(gps_data_string.value[15])*10+cti(gps_data_string.value[16])
                          +(float)(10*cti(gps_data_string.value[17])+cti(gps_data_string.value[18]))/60
                          +(float)cti(gps_data_string.value[20])/600
                          +(float)cti(gps_data_string.value[21])/6000
                          +(float)cti(gps_data_string.value[22])/60000
                          +(float)cti(gps_data_string.value[23])/600000
                          +(float)cti(gps_data_string.value[24])/6000000
                          +(float)cti(gps_data_string.value[25])/60000000;
  }else{
    Serial.println("GPS DATA NOT AVAILABLE!");
    tmp = false;
  }
  return(tmp);
}

