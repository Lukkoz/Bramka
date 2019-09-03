#include <Wire.h>
#define LED_INT 150
  byte output_buffer[16] = {0};
  byte pad_addresses[16]={0};
  byte padsConnected =0;
  
  void I2CSend(byte in,byte addr){
         
          Wire.write(in);
         
    }
   
    void I2CTransfere(byte addr,byte Nrec, byte command, byte R = 0,byte G = 0, byte B = 0){
         
              Wire.beginTransmission(addr);                            
              Wire.write(command);          
              Wire.write(R); 
              Wire.write(G); 
              Wire.write(B); 
              Wire.endTransmission();
              if(Nrec !=0){
                byte output[Nrec] = {0};
              
              byte out =0;
              Wire.requestFrom(addr,Nrec);
              while(Wire.available()){ 
                output_buffer[out]=Wire.read();
                out++;
                if(out == Nrec||out == 255)break;  
              }
           
            }
    }

    void IdentifyPads_I2C(bool add){
     byte error;
     for(byte tmpaddress  = 1; tmpaddress < 128; tmpaddress++){  //I2C scan
        Wire.beginTransmission(tmpaddress);
        Wire.write(0xFF);
        error = Wire.endTransmission(true);
      if (error == 0){
        Serial.print("I2C device found at address 0x");
        if (tmpaddress<16)
        Serial.print("0");
        Serial.print(tmpaddress,HEX);
        Serial.println("  !");
       if(add){ pad_addresses[padsConnected] = tmpaddress;
        padsConnected++;
        }
      }else if (error==4){
            Serial.print("Unknown error at address 0x");
            if (tmpaddress<16)
            Serial.print("0");
            Serial.println(tmpaddress,HEX);
      } 
      if(tmpaddress==127 && padsConnected == 0)Serial.println("No modules found.");   
    }
  }




void setup() {
  Wire.begin();
   Serial.begin(115200);
  IdentifyPads_I2C(true);
}

void loop() {
String input = " ";
  if(Serial.available()){
    input = Serial.readString();
    if(input == "ID"){
      IdentifyPads_I2C(false);
    }else if(input == "ON"){
      for(byte tt = 0; tt<padsConnected;tt++){
        I2CTransfere(pad_addresses[tt],0,0x0C,0,LED_INT,0); 
      }
    }else if(input == "OFF"){
      for(byte tt = 0; tt<padsConnected;tt++){
        I2CTransfere(pad_addresses[tt],0,0x0B); 
      }
    }else if(input == "TEST"){
      for(byte tt = 0; tt<padsConnected;tt++){
        I2CTransfere(pad_addresses[tt],4,0xCC); 
        Serial.print("Address:");
        Serial.println(pad_addresses[tt]);
        for(byte ii = 0; ii < 4;ii++)Serial.println(output_buffer[ii]);
      }
  }else if(input == "1"){
    I2CTransfere(pad_addresses[0],0,0xAD,LED_INT,0,0);
    I2CTransfere(pad_addresses[1],0,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if(input == "2"){
    I2CTransfere(pad_addresses[1],0,0xAD,LED_INT,0,0);
    I2CTransfere(pad_addresses[0],0,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }else if(input == "3"){
    I2CTransfere(pad_addresses[0],0,0xAD,0,LED_INT,0);
    I2CTransfere(pad_addresses[1],0,0xAD,0,LED_INT,0);
    Serial.println("OK");
  }

  
    
  }
  // put your main code here, to run repeatedly:

}
