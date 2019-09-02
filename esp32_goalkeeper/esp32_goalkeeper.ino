#include "Skribot.h"

SPIHandler *comm;
void setup()
{
	Serial.begin(115200);
	comm = new SPIHandler(SPI_PORT_1);
	comm->set_SPI_Settings(4000000, MSBFIRST, SPI_MODE0);
  comm->set_SPI_bit_format(8);
}

void loop()
{
	if(Serial.available()){
    char input = Serial.read();
    if(input == 'B'){
      comm->TransferAndReciveByte_8(1);
      Serial.println("Sending 1");
    }else if(input == 'C'){
      comm->TransferAndReciveByte_8(2);
      Serial.println("Sending 2");
    }else{
    
    }
}

}


