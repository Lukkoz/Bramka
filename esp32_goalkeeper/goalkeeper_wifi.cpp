#include "goalkeeper_wifi.h"

const char* ssid     = "GOAL_IOT";
const char* password = "";

WiFiServer server(80);

String header = "";

String Page_title = "IOT GOAL";
byte wifi_state = 0;

void wifi_end(){
	server.end();
}

void wifi_begin(){
	 WiFi.softAP(ssid, password);
	 IPAddress IP = WiFi.softAPIP();
	 Serial.println(IP);
	 server.begin();
}

byte wifi_check_for_client(){
	WiFiClient client = server.available();
	if(client){
		String currentLine = ""; 
		while (client.connected()) {            // loop while the client's connected
	      	if (client.available()) {             // if there's bytes to read from the client,
		        char c = client.read();             // read a byte, then
		        Serial.write(c);                    // print it out the serial monitor
		        header += c;
		        if (c == '\n') {                    // if the byte is a newline character
		          // if the current line is blank, you got two newline characters in a row.
		          // that's the end of the client HTTP request, so send a response:
		          if (currentLine.length() == 0) {
		            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
		            // and a content-type so the client knows what's coming, then a blank line:
		            client.println("HTTP/1.1 200 OK");
		            client.println("Content-type:text/html");
		            client.println("Connection: close");
		            client.println();
		            
		            // turns the GPIOs on and off
		            if (header.indexOf("GET /GAME_ONE/start") >= 0) {
		              wifi_state = 1;
		              Serial.println("GAME_ONE");
		            } else if (header.indexOf("GET /GAME_TWO/start") >= 0) {
		              wifi_state = 2;
		              Serial.println("GAME_TWO");
		            } else if (header.indexOf("GET /IDLE") >= 0) {
		              wifi_state = 0;
		              Serial.println("IDLE");
		            } 
		            
		            // Display the HTML web page
		            client.println("<!DOCTYPE html> GOAL DEMO <html>");
		            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		            client.println("<link rel=\"icon\" href=\"data:,\">");
		            // CSS to style the  buttons 
		            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
		            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
		            client.println(".button2 {background-color: #555555;}</style></head>");
		            
		            if(wifi_state == 0){
		            // Menu główne
			            client.println("<body><h1>Bramka Interaktywna</h1>");  
			            client.println("<p>GRA PIERWSZA</p>");      
			            client.println("<p><a href=\"/GAME_ONE/start\"><button class=\"button\">GAME 1</button></a></p>");
			            client.println("<p>GRA DRUGA</p>");
			     		client.println("<p><a href=\"/GAME_TWO/start\"><button class=\"button\">GAME 2</button></a></p>");
		         	}else if(wifi_state == 1 || wifi_state == 2){
		            	client.println("<body><h1>GAME GOAL:</h1>");
		            	if(wifi_state == 1){
		            		client.println("HIT THE BLUE SECTOR");
		            	}else{
		            		client.println("HIT ALL SECTORS IN LESS THEN 45 sec.");
		            	}
		            	client.println("<p><a href=\"/IDLE\"><button class=\"button button2\">END GAME</button></a></p>");
		        	}
		            client.println("</body></html>");
		            // The HTTP response ends with another blank line
		            client.println();
		            // Break out of the while loop
		            break;

		          }else { // if you got a newline, then clear currentLine
		            currentLine = "";
		          }
		        } else if(c != '\r'){  // if you got anything else but a carriage return character,
		          currentLine += c;      // add it to the end of the currentLine
		        }
	     	}else{
	     		break;
	     	}
	    }
		// Clear the header variable
	    header = "";
	    // Close the connection
	    client.stop();
	}
	return(wifi_state);
}

