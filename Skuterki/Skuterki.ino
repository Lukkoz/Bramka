#include "display.h"
#include "gps.h"
#include "gsm.h"

const char message[] = "\n{\"email\":\"a@a.com\",\"password\":\"string\"}\n";
const char message_1[] = "\n{\"lat\":\"1111\",\"long\":\"2222\"}\n";
void setup(){
	//init_dispaly();
	//init_gps();
	Serial.begin(115200);
}


void loop(){
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
			parse_json_from_buffer();
		}else if(tmp == 'P'){
			set_URL("https://api.sedaya.app/auth/login");
			post_data(message);
			print_buffer();
		}else if(tmp == 'L'){
			set_URL("https://api.sedaya.app/scooter/1");
			post_data(message_1);
			print_buffer();
		}
	}
	/*delay(5000);
	read_gps_data();
	Serial.print(current_gps_data.N,6);
	Serial.print(":");
	Serial.println(current_gps_data.E,6);
	*/
}


void runSerialPASS(){
	Serial.println("Serial passthroug active");
	Serial2.begin(9600);
	while(true){
		if(Serial.available()>0){
    		Serial2.write((char)Serial.read());
  		}
    	if(Serial2.available() >0){
    		Serial.print((char)Serial2.read());
  		}
	}
}
/*
AT+HTTPSET="URL","http://time.jsontest.com"  

OK
AT+HTTPACT=0  

OK

+HTTP: 1

+HTTPRES: <0>,<200>,<342> 
AT+HTTPREAD

OK

+HTTPREAD: 342
HTTP/1.1 200 OK
Access-Control-Allow-Origin: *
Content-Type: application/json
X-Cloud-Trace-Context: aa85a648a89dc60e93218b918552fffe
Date: Mon, 28 Jun 2021 19:16:24 GMT
Server: Google Frontend
Content-Length: 100
Connection: close

{
   "date": "06-28-2021",
   "milliseconds_since_epoch": 1624907784983,
   "time": "07:16:24 PM"
}
*/
