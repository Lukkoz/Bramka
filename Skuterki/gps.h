#ifndef GPS_H
#define GPS_H
#include <Arduino.h>
#include <TinyGPS++.h>
#define SERIAL_PORT_GPS Serial1

struct gps_data {
	float N;
	float E;
	byte hour;
	byte minute;
};

extern gps_data current_gps_data;

void init_gps();
gps_data read_gps_data();
void check_for_gps_data();
#endif

