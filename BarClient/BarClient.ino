
/*
MESSAGE FORMAT: [target_device ID][sending_device_ID][operation_ID]
SERVER_ID = 254
OPERATION ID:
*/
#define OPERATION_IDENTIFY  1
#define OPERATION_NOFIFY_CLIENT 2
#define OPERATION_NOTIFY_SERVER  3
#define OPERATION_UNNOTIFY_SERVER  4
#define OPERATION_SERVER_ACCEPT_NOTIFICATION 5


#define   PAD_ID_1 36
#define   PAD_ID_2 37
#define   PAD_ID_3 38
#define   PAD_ID_4 39

#define	  LED_PIN 10
#define	  BUZZER_PIN 4
#define	  BUTTON_PIN 9


#include "heltec.h"
#include "Adafruit_NeoPixel.h"
#define BAND    868E6  


int counter = 0;
byte deviceId = 2;
bool notified_server = false;
bool notification_server_accepted = false;
bool blinking_mode = false;
bool buzzer_active = false;
long notification_accept_time = 0;
long last_flip_time = 0;
long last_notification_send_time = 0;
bool led_state = false;


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
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(LED_PIN, OUTPUT);
	for (byte rr = 0; rr < 5;rr++) {
		led_on();
		delay(500);
		led_off();
		delay(500);
	}
	digitalWrite(BUZZER_PIN, LOW);
	//WIFI Kit series V1 not support Vext control
	Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
	LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
	//read_ID();
	
	
}
void led_on() {
	digitalWrite(LED_PIN, HIGH);
	led_state = true;
}

void led_off() {
	digitalWrite(LED_PIN, LOW);
	led_state = false;
}

void flip_led() {
	digitalWrite(LED_PIN, !led_state);
	led_state = !led_state;
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
		notification_accept_time = millis();
		last_flip_time = notification_accept_time;
		notified_server = true;
		if (notified_server) {
			blinking_mode = true;
			digitalWrite(BUZZER_PIN, HIGH);
		}
		break;
	case OPERATION_SERVER_ACCEPT_NOTIFICATION:
		notification_server_accepted = true;
		led_off();
		delay(500);
		led_on();
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
			Serial.println(operation_id);
			handleOperation(operation_id);
		}
		else {
			flushLoraData();
		}
	}
}

void loop() {

	if (!notified_server && digitalRead(BUTTON_PIN) == LOW) {
		long click_time = millis();	
		led_on();
		sendMessageToServer(OPERATION_NOTIFY_SERVER);
		notified_server = true;
		digitalWrite(BUZZER_PIN, HIGH);
		delay(500);
		digitalWrite(BUZZER_PIN, LOW);
		last_notification_send_time = millis();
	}
	else if (blinking_mode) {
		if (millis() - last_flip_time > 500) {
			flip_led();
			last_flip_time = millis();
		}
		if (millis() - notification_accept_time > 1500) {
			digitalWrite(BUZZER_PIN, LOW);
		}
		if (millis() - notification_accept_time > 60000) {
			notified_server = false;
			blinking_mode = false;
			notification_server_accepted = false;
		}
	}
	else if (notified_server && digitalRead(BUTTON_PIN) == LOW) {
		long click_time = millis();
		while (digitalRead(BUTTON_PIN) == LOW) {
			Serial.println(digitalRead(BUTTON_PIN));
			if (millis() - click_time > 3000) {
				led_off();
				sendMessageToServer(OPERATION_UNNOTIFY_SERVER);
				notified_server = false;
				notification_server_accepted = false;
				break;
			}
		}
		while (digitalRead(BUTTON_PIN) == LOW);
		delay(300);
	}
	else if (notified_server && !notification_server_accepted) {
		if (millis() - last_notification_send_time > 5000) {
			sendMessageToServer(OPERATION_NOTIFY_SERVER);
			last_notification_send_time = millis();
			led_off();
			delay(200);
			led_on();
		}
	}
	reciveMessage();
}