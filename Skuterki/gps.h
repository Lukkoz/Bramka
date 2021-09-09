#ifndef GPS_H
#define GPS_H
#include <Arduino.h>
#include "gsm.h"
#define GPS_STATUS_PIN 5

struct gps_data {
	float N;
	float E;
};

extern gps_data current_gps_data;

void init_gps();
bool read_gps_data();
#endif

