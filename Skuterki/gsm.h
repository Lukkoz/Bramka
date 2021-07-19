#ifndef GSM_H
#define GSM_H
#include "display.h"
#include <Arduino.h>
#include <ArduinoJson.h>


#define SERIAL_P Serial2

struct response {
	char *value;
	byte len;
};

void init_gsm();
void set_URL(char *url);
void read_URL_json();
void parse_json_from_buffer(byte lines_to_ommit);
byte readLine();
bool compare_line(const char* tmp);
bool send_cmd(const char *cmd,byte lines_to_read_extra = 0);
response get_value_after(const char* tmp,char end_marker = '\n');
void printResponse(response tmp);
void post_data(const char *message);
void print_buffer();
void read_JSON();
void updateServer(byte scooterID, float lat,float _long,byte batt,char *soundActive);
#endif