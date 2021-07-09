#ifndef GSM_H
#define GSM_H
#include <Arduino.h>

#define SERIAL_P Serial2

struct response {
	char *value;
	byte len;
};

void init_gsm();
void set_URL(char *url);
void read_URL_json();
void parse_json_from_buffer();
byte readLine();
bool compare_line(const char* tmp);
bool send_cmd(const char *cmd,byte lines_to_read_extra = 0);
response get_value_after(const char* tmp,char end_marker = '\n');
void printResponse(response tmp);
void post_data(const char *message);
void print_buffer();

#endif