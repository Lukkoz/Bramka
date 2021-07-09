#include "gsm.h"

char buffer[500];
char line_buffer[100];

bool send_cmd(const char* cmd,byte lines_to_read_extra){
	bool operation = false;
	SERIAL_P.println(cmd);
	readLine();//comnad echo
	if(lines_to_read_extra != 255){
	readLine();//OK
	if(line_buffer[0] != 'O' || line_buffer[1] != 'K'){
		Serial.print("Fail sending:");
		Serial.println(cmd);
		return(operation);
	}
	operation = true;
	for(byte rr =0; rr < lines_to_read_extra;rr++)readLine();
	}
	return(operation);
}

void init_gsm(){
	SERIAL_P.begin(9600);
	send_cmd("AT");
	send_cmd("AT+CMEE=2");
	if(send_cmd("at+mipcall=1,\"internet\"",2)){
		response ip = get_value_after("+MIPCALL: ");
		Serial.println("System_working with IP:");
		Serial.println(ip.value);
	}else{
		Serial.println("Abort init");
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
	char tmp[100];
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
	readLine();
	readLine();
	response data = get_value_after("\"date\": ",',');
	readLine();
	response millis = get_value_after("since_epoch\": ",',');
	readLine();
	response time = get_value_after("time\": ");
	readLine();

	Serial.print("Data: ");
	printResponse(data);
	Serial.print("Czas: ");
	printResponse(time);
}

void post_data(const char *message){
	char tmp[100];
	int len = strlen(message);
	send_cmd("AT+HTTPSET=\"CONTYPE\",\"application/json\"");
	sprintf(tmp,"AT+HTTPDATA=%d",len);
	send_cmd(tmp,255);
	send_cmd(message,255);
	delay(200);
	sprintf(tmp,"AT+HTTPACT=1,%d",len);
	send_cmd(tmp,5);
	send_cmd("AT+HTTPREAD",2);
	print_buffer();

}
void print_buffer(){
	while(SERIAL_P.available()>0){
		readLine();
	}
}

