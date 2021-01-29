#include <WiFi.h>

const char* ssid     = "BramkaZaporX";
const char* password = "ZaparciaDamiana";

WiFiServer server(80);

String header = "";

String Page_title = "BramkaZaporX - hit cenowy";
byte state = 0;


void wifi_begin();
byte wifi_check_for_client();