#include <Arduino.h>
#include "gsm.h"

#define DEBUG_MODE

char buffer[500];
char line_buffer[100];
char json_buffer[500];
StaticJsonDocument<200> doc;
void  debug_print(const char* msg){
	#ifdef DEBUG_MODE
	Serial.println(msg);
	#endif
}

bool send_cmd(const char* cmd,byte lines_to_read_extra,byte important_line){
	bool operation = false;
	SERIAL_P.println(cmd);
	readLine();//comnad echo
	for(byte rr =1; rr < lines_to_read_extra+1;rr++){
		readLine(rr==important_line);
	}
	return(true);
}

void sendCTRLZ(){
	SERIAL_P.write(26); //"ctrl+z"
}

void performHTTPrequest(const char* tmp){
	send_cmd("AT+HTTPTERM");
	send_cmd("AT+HTTPINIT");
	send_cmd("AT+CHTTPACT=\"api.sedaya.app\",80");
	send_cmd(tmp);
	sendCTRLZ();
}

void performHTTPrequest(const char* url,const char* request_type,bool Authorization,char *msg){
	char tmp_request[1000];
	byte reequest_counter = 0;
	if(!Authorization){
	sprintf(tmp_request,"%s %s HTTP/1.1\r\nHost: scooter.com\r\nUser-Agent: scooter\r\nContent-Type:application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",request_type,url,strlen(msg),msg);
	}
	send_cmd("AT+HTTPTERM");
	send_cmd("AT+HTTPINIT");
	send_cmd("AT+CHTTPACT=\"api.sedaya.app\",80");
	send_cmd(tmp_request,0); // number of new lines in http request
	sendCTRLZ();
	while(true){
		readLine(true);
		if(contains("DATA"))break;
	}
	debug_print("END OF THE REQUEST");

}

void init_gsm(){
	SERIAL_P.begin(115200, SERIAL_8N1, 13, 15);
	while(SERIAL_P.available()){
		Serial.print(SERIAL_P.read());
	}
	send_cmd("AT");
	send_cmd("AT+CMEE=2");
	if(send_cmd("AT+CGDCONT=1,\"IP\",\"internet\"")){
		send_cmd("AT+NETOPEN",3,3);
		send_cmd("AT+IPADDR",3,1);
		response ip = get_value_after("+IPADDR: ");
		#ifndef DISPLAY_DEBUG
		Serial.println("*****************************");
		Serial.println("System_working with IP:");
		printResponse(ip);
		print_on_display("READY");
		#else
		print_on_display(ip.value);
		#endif
	}else{
		#ifndef DISPLAY_DEBUG
		Serial.println("Abort init");
		#else
		print_on_display("Init done skipping");
		#endif
	}
}


byte readLine(bool save){
	byte line_index = 0;
	bool eol = false;
	while(!eol){
		while(SERIAL_P.available() > 0){
			char tmp = SERIAL_P.read();
			if(save)line_buffer[line_index] = tmp;
			#ifdef DEBUG_MODE
				Serial.print(tmp);
			#endif
			if(tmp == '\n'){
				eol = true;
				break;
			}else{
				if(save)line_index++;
			}
		}
	}
	return(line_index);
}

bool compare_line(const char* tmp){
	return(strcmp(line_buffer,tmp) == 0);
}
response get_value_after(const char* tag, char end_marker){
	response output;
	char *tmp = strstr(line_buffer,tag);
	byte startpoint = tmp-line_buffer+strlen(tag);
	char tmp_c;
	byte ii = 0;
	while(true){
	  tmp_c = line_buffer[startpoint+ii];
	  if(tmp_c == end_marker)break;
	  ii++;
 	}
 	output.len = ii;
 	output.value = new char[ii];
 	for(byte dd = 0; dd <ii;dd++){
 		output.value[dd] = line_buffer[startpoint+dd];
 	}
 	return(output);
}

bool contains(const char* tag){
	char *outcome = strstr(line_buffer,tag);
	return(outcome != NULL);
}


void set_URL(char *url){
	char tmp[200];
	sprintf(tmp,"AT+HTTPSET=\"URL\",\"%s\"",url);
	send_cmd(tmp);
}
void read_URL_json(){
	send_cmd("AT+HTTPACT=0",4);
	send_cmd("AT+HTTPREAD",11);
}

void printResponse(response tmp){
	for(byte rr = 0; rr<tmp.len;rr++)Serial.print(tmp.value[rr]);
	Serial.println();
}
void parse_json_from_buffer(){
	char tmp;
	byte counter = 0;
	while(true){
		if(SERIAL_P.available() >0){
			tmp = SERIAL_P.read();
		}
		if(tmp == '\r' && counter == 0){
			counter++;
		}else if(tmp == '\n' && counter == 1){
			counter++;
		}else if(tmp == '\r' && counter == 2){
			counter++;
		}else if(tmp == '\n' && counter == 3){
			break;
		}else{
			counter = 0;
		}
	}
	debug_print("END OF HEADER");
	read_JSON();
	debug_print("JSON LOADAED");
	while(true){
		readLine(true);
		if(contains("+CHTTPACT: 0"))break;
	}
	DeserializationError error = deserializeJson(doc, json_buffer);
  	if (error) {
	    Serial.print(F("deserializeJson() failed: "));
	    Serial.println(error.f_str());
	    return;
 	}


}

void print_buffer(){
	while(SERIAL_P.available()>0){
		readLine();
	}
}

void read_JSON(){
	int bracket_counter = 0;
	int line_index = 0;
	bool opened = false;
 	while(true){
		if(SERIAL_P.available() > 0){
			char tmp  = SERIAL_P.read();
			if(tmp == '{'){
				opened = true;
				bracket_counter++;
			}else if(tmp == '}'){
				bracket_counter--;
			}
			if(opened){
				json_buffer[line_index] = tmp;
				line_index++;
			}
		}
		if(opened && bracket_counter == 0)break;
	}
}

StaticJsonDocument<200> updateServerScooter(const char *scooterID, float _lat,float _long,byte batt,char *soundActive,char *openTrunk,char *isCharging){
		char tmp_m[200];
		char tmp_url[100];
		char lat_strg[10];
		char long_strg[10];
		dtostrf(_long,0,6,long_strg);
		dtostrf(_lat,0,6,lat_strg);
		sprintf(tmp_m,"{\"lat\":\"%s\",\"long\":\"%s\",\"soundActive\":%s,\"openTrunk\":%s,\"isCharging\":%s,\"battery\":{\"chargeLevel\":%d}}",lat_strg,long_strg,soundActive,openTrunk,isCharging,batt);
		sprintf(tmp_url,"https://api.sedaya.app/devices/scooters/%s",scooterID);
		Serial.println(tmp_m);
		performHTTPrequest(tmp_url,"POST",false,tmp_m);
		parse_json_from_buffer();
		return(doc);
}

StaticJsonDocument<200> updateServerScooter(const char *scooterID, float _lat,float _long,byte batt){
		char tmp_m[200];
		char tmp_url[100];
		char lat_strg[10];
		char long_strg[10];
		dtostrf(_long,0,6,long_strg);
		dtostrf(_lat,0,6,lat_strg);
		sprintf(tmp_m,"{\"lat\":\"%s\",\"long\":\"%s\",\"battery\":{\"chargeLevel\":%d}}",lat_strg,long_strg,batt);
		sprintf(tmp_url,"https://api.sedaya.app/devices/scooters/%s",scooterID);
		Serial.println(tmp_m);
		performHTTPrequest(tmp_url,"POST",false,tmp_m);
		parse_json_from_buffer();
		return(doc);
}

void updateServerStation(const char *scooterID){
		char tmp_m[200];
		char tmp_url[100];
		sprintf(tmp_m,"\n{\"scooters\":[{\"id\":\"%s\"}]}\n",scooterID);
		sprintf(tmp_url,"https://api.sedaya.app/devices/stations/%d",1);
		performHTTPrequest(tmp_url,"POST",false,tmp_m);
		while(true){
		readLine(true);
		if(contains("+CHTTPACT: 0"))break;
		}
}

int cti(char x){
  int y = x - '0';
  return(y);
}