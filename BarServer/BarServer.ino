#define	  LED_PIN 27
#define	  BUZZER_PIN 21
#define	  BUTTON_PIN 22

#define OPERATION_IDENTIFY  1
#define OPERATION_NOFIFY_CLIENT 2
#define OPERATION_NOTIFY_SERVER  3

#define CLIENT_STATUS_NOT_CONECTED 0
#define CLIENT_STATUS_IDLE 1
#define CLIENT_STATUS_NOTIFIED 2
#define CLIENT_STATUS_ACCEPTED_NOTIFICATION 3
#define CLIENT_STATUS_CALL_BAR 4


#include "heltec.h"
#include "Adafruit_NeoPixel.h"
#define N_CLIENTS 2
#define BAND    868E6  
byte button_GPIO[N_CLIENTS] = {35,23};
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(N_CLIENTS, LED_PIN, NEO_GRB + NEO_KHZ800);
byte client_status[N_CLIENTS] = { CLIENT_STATUS_NOT_CONECTED };
bool soundAlarm = false;

void confirmIdentification(byte Id) {
	Serial.print("Confirming connection to ");
	Serial.println(Id);
	if (client_status[Id - 1] == CLIENT_STATUS_NOT_CONECTED) {
		client_status[Id - 1] = CLIENT_STATUS_IDLE;
	}
	updateBarNotofications();
	
}

void sendMessageToClient(byte clientId,byte operation) {
	Serial.print("Sending packet:");
	Serial.println(clientId);
	Serial.println(254);
	Serial.println(operation);

	LoRa.beginPacket();
	LoRa.write(clientId);
	LoRa.write(254);
	LoRa.write(operation);
	LoRa.endPacket();
}

void handleServerNotification(byte messageHost) {
	switch (client_status[messageHost-1])
	{
	case CLIENT_STATUS_IDLE:
		client_status[messageHost - 1] = CLIENT_STATUS_CALL_BAR;
		soundAlarm = true;
		break;
	case CLIENT_STATUS_NOTIFIED:
		client_status[messageHost - 1] = CLIENT_STATUS_ACCEPTED_NOTIFICATION;
		break;
	default:
		break;
	}
	updateBarNotofications();
}
void setupButtons() {
	for (byte i = 0; i < N_CLIENTS; i++)
	{
		pinMode(button_GPIO[i], INPUT_PULLUP);
	}

}

byte getClickedButtonID() {
	for (byte i = 0; i < N_CLIENTS; i++)
	{
		if (digitalRead(button_GPIO[i]) == LOW) {
			return(i);
		}
	}
	return(0);

}

void handleButtonClick(byte Id) {
	soundAlarm = false;
	switch (client_status[Id])
	{
	case CLIENT_STATUS_IDLE:
		sendMessageToClient(Id + 1, OPERATION_NOFIFY_CLIENT);
		client_status[Id] = CLIENT_STATUS_NOTIFIED;
		break;
	case CLIENT_STATUS_CALL_BAR:
		client_status[Id] = CLIENT_STATUS_IDLE;
		break;
	case CLIENT_STATUS_ACCEPTED_NOTIFICATION:
		client_status[Id] = CLIENT_STATUS_IDLE;
		break;
	case CLIENT_STATUS_NOTIFIED:
		sendMessageToClient(Id + 1, OPERATION_NOFIFY_CLIENT);
		break;
	default:
		break;
	}
	updateBarNotofications();
}

void setupLed(byte status, byte Id) {
	switch (status)
	{
	case CLIENT_STATUS_NOT_CONECTED:
		pixels.setPixelColor(Id, pixels.Color(255, 0, 0));
		break;
	case CLIENT_STATUS_IDLE:
		pixels.setPixelColor(Id, pixels.Color(255, 255, 255));
		break;
	case CLIENT_STATUS_NOTIFIED:
		pixels.setPixelColor(Id, pixels.Color(0, 255, 0));
		break;
	case CLIENT_STATUS_CALL_BAR:
		pixels.setPixelColor(Id, pixels.Color(0, 0, 255));
		break;
	case CLIENT_STATUS_ACCEPTED_NOTIFICATION:
		pixels.setPixelColor(Id, pixels.Color(0, 255, 255));
		break;
	default:
		break;
	}
}

void updateBarNotofications() {
	Serial.println("CLIENT STATUS:");
	for (int i = 0; i < N_CLIENTS; i++)
	{
		Serial.print(client_status[i]);
		setupLed(client_status[i], i);
	}
	Serial.println();
	pixels.show();
	
	
}



bool reciveMessage() {
	int packetSize = LoRa.parsePacket();
	if (packetSize) {
		// received a packet
		Serial.print("Received packet!");
		byte TargetID = LoRa.read();
		if (TargetID == 254) {
			byte messageHost = LoRa.read();
			byte operation_id = LoRa.read();
			handleOperation(messageHost,operation_id);
			return(true);
		}
	}
	return(false);
}

void handleOperation(byte messageHost, byte operation_id) {
	switch (operation_id)
	{
	case OPERATION_IDENTIFY:
		confirmIdentification(messageHost);
		break;
	case OPERATION_NOTIFY_SERVER:
		handleServerNotification(messageHost);
		break;
	default:
		break;
	}
}

void askForIdentification() {
	for (byte i = 0; i < N_CLIENTS; i++)
	{
		sendMessageToClient(i + 1, OPERATION_IDENTIFY);
		long ask_time = millis();
		while (!reciveMessage()) {
			if (millis() - ask_time > 2000) {
				break;
			}
		}

	}
}

void setup() {
	Serial.begin(115200);
	//WIFI Kit series V1 not support Vext control
	Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
	LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
	setupButtons();
	Serial.println("INIT OK!");
	askForIdentification();
}

void loop() {
	//byte button_clicked = getClickedButtonID();
	/*if (button_clicked != 0) {
		handleButtonClick(button_clicked);
	}
	*/
	if (Serial.available()) {
		handleButtonClick(1);
		Serial.read();
	}
	reciveMessage();
	
}
