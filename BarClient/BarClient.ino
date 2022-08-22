
/*
MESSAGE FORMAT: [target_device ID][sending_device_ID][operation_ID]
SERVER_ID = 254
OPERATION ID:
*/
#define OPERATION_IDENTIFY  1
#define OPERATION_NOFIFY_CLIENT 2
#define OPERATION_NOTIFY_SERVER  3


#define   PAD_ID_1 9
#define   PAD_ID_2 8
#define   PAD_ID_3 7
#define   PAD_ID_4 6

#define	  LED_PIN 20
#define	  BUZZER_PIN 21
#define	  BUTTON_PIN 15


#include "heltec.h"
#include "Adafruit_NeoPixel.h"
#define NUMPIXELS 2
#define BAND    868E6  
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int counter = 0;
byte deviceId = 2;


void read_ID() {								// reading the ID from hardware register
	pinMode(PAD_ID_1, INPUT_PULLUP);
	pinMode(PAD_ID_2, INPUT_PULLUP);
	pinMode(PAD_ID_3, INPUT_PULLUP);
	pinMode(PAD_ID_4, INPUT_PULLUP);
	byte a1 = (byte)digitalRead(PAD_ID_1);
	byte a2 = (byte)digitalRead(PAD_ID_2);
	byte a3 = (byte)digitalRead(PAD_ID_3);
	byte a4 = (byte)digitalRead(PAD_ID_4);
	deviceId = 1 * a1 + 2 * a2 + 4 * a3 + 8 * a4;
}

void setup() {
	Serial.begin(115200);
	//WIFI Kit series V1 not support Vext control
	Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
	LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
	//read_ID();
	pinMode(BUTTON_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pixels.begin();
	
}

void sendMessageToServer(byte operation) {
	Serial.print("Sending packet:");
	Serial.println(254);
	Serial.println(deviceId);
	Serial.println(operation);
	
	LoRa.beginPacket();
	LoRa.write(254);
	LoRa.write(deviceId);
	LoRa.write(operation);
	LoRa.endPacket();
}

void flushLoraData() {
	while (LoRa.available())
	{
		LoRa.read();
	}
}

void handleOperation(byte O_ID) {
	switch (O_ID)
	{
	case OPERATION_IDENTIFY:
		sendMessageToServer(OPERATION_IDENTIFY);
		break;
	case OPERATION_NOFIFY_CLIENT:
		Serial.println("CLIENT NOTIFICATION!");
		break;
	default:
		break;
	}
}

void reciveMessage() {
	int packetSize = LoRa.parsePacket();
	if (packetSize) {
		// received a packet
		Serial.println("Received packet:");
		byte TargetID = LoRa.read();
		if (TargetID == deviceId) {
			byte messageHost = LoRa.read();
			if (messageHost != 254) {
				flushLoraData();
				return;
			}
			byte operation_id = LoRa.read();
			handleOperation(operation_id);
		}
		else {
			flushLoraData();
		}
	
		
	
	}
}

void loop() {
	if (Serial.available()) {
		sendMessageToServer(OPERATION_NOTIFY_SERVER);
		Serial.read();
		delay(1000);
	}
	
	reciveMessage();



}