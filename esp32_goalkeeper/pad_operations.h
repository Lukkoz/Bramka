#ifndef PAD_OPS
#define PAD_OPS
#include <Arduino.h>
#define DEBUG_MODE

#define RS_MODE_PIN 21

#define MIN_REACTION_LEVEL 20
#define N_SAMPLE 15
#define PADS_CONNECTED 12

//PAD REQUESTS:
#define RED 1
#define GREEN 2
#define BLUE 3
#define REACTION_RED 4
#define REACTION_BLUE 5
#define REACTION_GREEN 6
#define DISABLE_REACTION 7
#define OFF 8
#define REACTION_TIME_1000 9
#define REACTION_TIME_2000 10
#define REACTION_TIME_500 11
#define REACTION_CHANGE 13
#define WHITE 14
#define TRESHHOLD_SET 17
#define RAPORT_READOUT 18
#define READ_TEST 19
//reaction commands - out of use for newest version


void enter_transmit_mode();
void enter_recive_mode();
void SendMessage(byte addr,byte command);
void set_all(byte req);
void setPanel(byte panelId,byte requestID);
void read_from_panel(byte panelId,byte command,byte nbytesToRead);
byte raport_pad_status(bool debug_mode = false);

#endif