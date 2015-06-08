#include <OneWire.h>

#define DATA_LED_PIN  4
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary)

void setup(void) {
  //Serial.begin(9600);// default for terminal
  Serial.begin(19200);// for my bt module
  pinMode(DATA_LED_PIN, OUTPUT);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  String result = "";
  char val;
  //int flag;

  if(Serial.available() > 0){     
    val = Serial.read();
    //Serial.println(">");   
    //Serial.println(val);
    //Serial.println("<");
    //flag=0;
    
    if (val == '1' || val == '0'){
      if (val == '1'){
        digitalWrite(DATA_LED_PIN, HIGH);
        delay(100);
        digitalWrite(DATA_LED_PIN, LOW);
      }
      
      result += "BEGIN";
    
      while(1){
        if ( !ds.search(addr)) {
          //Serial.println("No more addresses.");
          //Serial.println();
          ds.reset_search();
          delay(250);
          break;
        }
      
        //Serial.print("ROM =");
        result += "ROM";
        for( i = 0; i < 8; i++) {
          //Serial.write(' ');
          //Serial.print(addr[i], HEX);
          
          if (addr[i]<16)
            result += "0";
          result += String(addr[i], HEX);
        }
      
        if (OneWire::crc8(addr, 7) != addr[7]) {
            //Serial.println("CRC is not valid!");
            result += 'ERROR';
            return;
        }
        //Serial.println();
      
        // the first ROM byte indicates which chip
        switch (addr[0]) {
          case 0x10:
            //Serial.println("  Chip = DS18S20");  // or old DS1820
            type_s = 1;
            break;
          case 0x28:
            //Serial.println("  Chip = DS18B20");
            type_s = 0;
            break;
          case 0x22:
            //Serial.println("  Chip = DS1822");
            type_s = 0;
            break;
          default:
            //Serial.println("Device is not a DS18x20 family device.");
            return;
        } 
      
        ds.reset();
        ds.select(addr);
        ds.write(0x44);        // start conversion, use ds.write(0x44,1) with parasite power on at the end
    
        delay(2000);     // maybe 750ms is enough, maybe not
        // we might do a ds.depower() here, but the reset will take care of it.
      
        present = ds.reset();
        ds.select(addr);    
        ds.write(0xBE);         // Read Scratchpad
      
        //Serial.print("  Data = ");
        //Serial.print(present, HEX);
        //Serial.print(" ");
        for ( i = 0; i < 9; i++) {           // we need 9 bytes
          data[i] = ds.read();
          //Serial.print(data[i], HEX);
          //Serial.print(" ");
        }
        //Serial.print(" CRC=");
        //Serial.print(OneWire::crc8(data, 8), HEX);
        //Serial.println();
      
        // Convert the data to actual temperature
        // because the result is a 16 bit signed integer, it should
        // be stored to an "int16_t" type, which is always 16 bits
        // even when compiled on a 32 bit processor.
        int16_t raw = (data[1] << 8) | data[0];
        if (type_s) {
          raw = raw << 3; // 9 bit resolution default
          if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
          }
        } else {
          byte cfg = (data[4] & 0x60);
          // at lower res, the low bits are undefined, so let's zero them
          if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
          else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
          else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
          //// default is 12 bit resolution, 750 ms conversion time
        }
        celsius = (float)raw / 16.0;
        fahrenheit = celsius * 1.8 + 32.0;
        //Serial.print("  Temperature = ");
        //Serial.print(celsius);
        //Serial.print(" Celsius, ");
        //Serial.print(fahrenheit);
        //Serial.println(" Fahrenheit");
        result += celsius;
      }
      
      result += "END#";    
            
      //if(flag == 0){
        Serial.println(result);
        //flag = 1;
      //}
    }
  }
}
