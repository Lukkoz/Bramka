#ifndef GSM_H
#define GSM_H
#include "display.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#define STATION


#define SERIAL_P Serial2

struct response {
	char *value;
	byte len;
};

void init_gsm();
void set_URL(char *url);
void read_URL_json();
void parse_json_from_buffer();
byte readLine(bool save = false);
bool compare_line(const char* tmp);
bool contains(const char* tag);
int cti(char x);
bool send_cmd(const char *cmd,byte lines_to_read_extra = 1,byte important_line = 0);
response get_value_after(const char* tmp,char end_marker = '\n');
void printResponse(response tmp);
void print_buffer();
void read_JSON();
StaticJsonDocument<200> updateServerScooter(const char *scooterID, float lat,float _long,byte batt,char *soundActive,char *openTrunk,char *isCharging);
StaticJsonDocument<200> updateServerScooter(const char *scooterID, float lat,float _long,byte batt);
void updateServerStation(const char *scooterID);
void performHTTPrequest(const char* url,const char* request_type,bool Authorization,char *msg);
#endif