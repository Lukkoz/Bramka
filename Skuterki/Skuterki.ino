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
