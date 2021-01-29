#include "goalkeeper_wifi.h"
void wifi_begin(){
	 WiFi.softAP(ssid, password);
	 IPAddress IP = WiFi.softAPIP();
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
		              state = 1;
		              Serial.println("GAME_ONE");
		            } else if (header.indexOf("GET /GAME_TWO/start") >= 0) {
		              state = 2;
		              Serial.println("GAME_TWO");
		            } else if (header.indexOf("GET /IDLE") >= 0) {
		              state = 0;
		              Serial.println("IDLE");
		            } 
		            
		            // Display the HTML web page
		            client.println("<!DOCTYPE html><html>");
		            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		            client.println("<link rel=\"icon\" href=\"data:,\">");
		            // CSS to style the  buttons 
		            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
		            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
		            client.println(".button2 {background-color: #555555;}</style></head>");
		            
		            if(state == 0){
		            // Menu główne
			            client.println("<body><h1>Bramka ZaporX</h1>");  
			            client.println("<p>GRA PIERWSZA - USZCZEL JEDNEGO</p>");      
			            client.println("<p><a href=\"/GAME_ONE/start\"><button class=\"button\">GRA 1</button></a></p>");
			            client.println("<p>GRA DRUGA - ZAPIERDOL KAZDEMU</p>");
			     		client.println("<p><a href=\"/GAME_TWO/start\"><button class=\"button\">GRA 2</button></a></p>");
		         	}else if(state == 1 || state == 2){
		            	client.println("<body><h1>GRASZ W GRE:</h1>");
		            	if(state == 1){
		            		client.println("ZAPIERDOL JEDNEGO BAGIETE!");
		            	}else{
		            		client.println("ZAPIERDL WSZYSTKIE BAGIETY");
		            	}
		            	client.println("<p><a href=\"/IDLE\"><button class=\"button button2\">QNIEC GRY</button></a></p>");
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
	     	}
	    }
		// Clear the header variable
	    header = "";
	    // Close the connection
	    client.stop();
	}
}

