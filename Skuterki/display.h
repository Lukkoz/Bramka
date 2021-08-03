#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "gsm.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


void init_display();

void print_on_display(char *tmp);

/*
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();
*/
#endif