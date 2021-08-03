#include <Arduino.h>
#include "gsm.h"
#define STATION

char buffer[500];
char line_buffer[100];
char json_buffer[500];
StaticJsonDocument<200> doc;

bool send_cmd(const char* cmd,byte lines_to_read_extra){
	bool operation = false;
	SERIAL_P.println(cmd);
	readLine();//comnad echo
	if(lines_to_read_extra != 255){
	readLine();//OK
	if(line_buffer[0] != 'O' && line_buffer[1] != 'K'){
		Serial.println("Fail.");
		return(operation);
	}
	for(byte rr =0; rr < lines_to_read_extra;rr++)readLine();
	}
	return(operation);
}

void init_gsm(){
	SERIAL_P.begin(9600, SERIAL_8N1, 13, 15);
	print_on_display("INIT 1");
	delay(3000);
	print_on_display("INIT 2");
	while(SERIAL_P.available()){
		Serial.print(SERIAL_P.read());
	}
	send_cmd("AT");
	send_cmd("AT+CMEE=2");
	if(send_cmd("at+mipcall=1,\"internet\"",2)){
		response ip = get_value_after("+MIPCALL: ");
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

byte readLine(){
	byte line_index = 0;
	bool eol = false;
	while(!eol){
		while(SERIAL_P.available() > 0){
			line_buffer[line_index] = SERIAL_P.read();
			Serial.print(line_buffer[line_index]);
			if(line_buffer[line_index] == '\n'){
				eol = true;
				break;
			}else{
				line_index++;
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
void parse_json_from_buffer(byte lines_to_ommit){
	for(byte rr=0;rr<lines_to_ommit;rr++)readLine(); //Ommiting the HTTP Header
	read_JSON();
	DeserializationError error = deserializeJson(doc, json_buffer);
  	if (error) {
	    Serial.print(F("deserializeJson() failed: "));
	    Serial.println(error.f_str());
	    return;
 	}

}

void post_data(const char *message){
	char tmp[100];
	int len = strlen(message);
	send_cmd("AT+HTTPSET=\"CONTYPE\",\"application/json\"");
	Serial.println(message);
	sprintf(tmp,"AT+HTTPDATA=%d",len);
	send_cmd(tmp,255);
	send_cmd(message,255);
	delay(200);
	sprintf(tmp,"AT+HTTPACT=1,%d",10);
	send_cmd(tmp,5);
	send_cmd("AT+HTTPREAD",255);

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
		while(SERIAL_P.available() > 0){
			json_buffer[line_index] = SERIAL_P.read();
			if(json_buffer[line_index] == '{'){
				opened = true;
				bracket_counter++;
			}else if(json_buffer[line_index] == '}'){
				bracket_counter--;
			}
			line_index++;
		}
		if(opened && bracket_counter == 0)break;
	}
}

StaticJsonDocument<200> updateServerScooter(const char *scooterID, float _lat,float _long,byte batt,char *soundActive,char *openTrunk){
		char tmp_m[200];
		char tmp_url[100];
		char lat_strg[10];
		char long_strg[10];
		dtostrf(_long,0,6,long_strg);
		dtostrf(_lat,0,6,lat_strg);
		sprintf(tmp_m,"\n{\"lat\":\"%s\",\"long\":\"%s\",\"soundActive\":%s,\"openTrunk\":%s,\"battery\":{\"chargeLevel\":%d}}\n",lat_strg,long_strg,soundActive,openTrunk,batt);
		sprintf(tmp_url,"https://api.sedaya.app/devices/scooters/%s",scooterID);
		Serial.println(tmp_m);
		set_URL(tmp_url);
		post_data(tmp_m);
		parse_json_from_buffer(16);
		return(doc);
}

StaticJsonDocument<200> updateServerScooter(const char *scooterID, float _lat,float _long,byte batt){
		char tmp_m[200];
		char tmp_url[100];
		char lat_strg[10];
		char long_strg[10];
		dtostrf(_long,0,6,long_strg);
		dtostrf(_lat,0,6,lat_strg);
		sprintf(tmp_m,"\n{\"lat\":\"%s\",\"long\":\"%s\",\"battery\":{\"chargeLevel\":%d}}\n",lat_strg,long_strg,batt);
		sprintf(tmp_url,"https://api.sedaya.app/devices/scooters/%s",scooterID);
		Serial.println(tmp_m);
		set_URL(tmp_url);
		post_data(tmp_m);
		parse_json_from_buffer(16);
		return(doc);
}

void updateServerStation(const char *scooterID){
		char tmp_m[200];
		char tmp_url[100];
		sprintf(tmp_m,"\n{\"scooters\":[{\"id\":\"%s\"}]}\n",scooterID);
		sprintf(tmp_url,"https://api.sedaya.app/devices/stations/%d",1);
		set_URL(tmp_url);
		post_data(tmp_m);
		for(byte rr = 0; rr<14;rr++)readLine();
		
}