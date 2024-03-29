#ifndef PAD_OPS
#define PAD_OPS
#include <Arduino.h>
#define DEBUG_MODE

#define RS_MODE_PIN 21

#define MIN_REACTION_LEVEL 25
#define COOLDOWN_LEVEL 25
#define N_SAMPLE 50
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
#define SET_INTENSITIVITY_0 9
#define SET_INTENSITIVITY_1 10
#define SET_INTENSITIVITY_2 11
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
int read_from_panel(byte panelId,byte command,byte nbytesToRead);
byte raport_pad_status(bool debug_mode = false);
void cooldown_after_hit();
void raport_pads(bool hit_only = false);

#endif