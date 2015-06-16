void readTempDS18B20(void){   
  counter = 0;  
  while(1){ // Main loop START
    addr = ds18b20Sensors[counter].addr;
    if ( !ds.search(addr)) { // No more addresses.
      ds.reset_search();
      delay(250);
      break;
    }
      
    ds18b20Sensors[counter].valid = true;    
    ds18b20Sensors[counter].rom = "";
      
    if (OneWire::crc8(addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      ds18b20Sensors[counter].valid = false;
      return;
    }

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
    } // switch
      
    ds.reset();
    ds.select(addr);
    ds.write(0x44);  // start conversion, use ds.write(0x44,1) with parasite power on at the end
    
    delay(2000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
      
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);  // Read Scratchpad
      
    //Serial.print("  Data = ");
    //Serial.print(present, HEX);
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
    }
      
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
      if (cfg == 0x00) raw = raw & ~7;      // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    ds18b20Sensors[counter].celsius = (float)raw / 16.0;
    ds18b20Sensors[counter].fahrenheit = ds18b20Sensors[counter].celsius * 1.8 + 32.0;
    counter++;
    if(counter >= MAX_DS18B20){
      break;
    }
  }// Main loop END
  
  for(; counter<MAX_DS18B20; ++counter){
    ds18b20Sensors[counter].valid = false;    
  }
}

void printTemp(void){
  lcd.clear();
  if (screen == SCREEN_DS18B20){
    for(i=0; i<MAX_DS18B20; ++i){
      if(ds18b20Sensors[i].valid){
        lcd.setCursor(0, i);
        tmpString = "";                
        tmpString += ds18b20Sensors[i].celsius;
        tmpString += " ";
        ds18b20Sensors[i].rom = "";
        for(j=0; j<4; ++j){
          if (ds18b20Sensors[i].addr[j]<16)
            ds18b20Sensors[i].rom += "0";
          ds18b20Sensors[i].rom += String(ds18b20Sensors[i].addr[j], HEX);
        }
        tmpString += ds18b20Sensors[i].rom;
        lcd.print(tmpString);
      }
    } 
  } else if (screen == SCREEN_DHT){
    tmpString = "T:";
    tmpString += DHT_1.temperature;
    tmpString += " H:";
    tmpString += DHT_1.humidity;    
    lcd.setCursor(0,0);
    lcd.print(tmpString);
  }
  
  // print "time"
  lcd.setCursor(0,2);
  tmpString = "Ticks:";
  tmpString += tick;
  lcd.print(tmpString);
  lcd.setCursor(0,3);
  h = tick/60;
  m = tick - h*60;
  d = h/24;
  h = h - d*24;
  // For tick testing
  // 1 hour = 6 minutes
  // 1 day = 2 hours
//  h = tick/6;
//  m = tick - h*6;
//  d = h/2;
//  h = h - d*2;
  tmpString = "D:";
  tmpString += d;
  tmpString += " H:";
  tmpString += h;
  tmpString += " M:";
  tmpString += m;
  lcd.print(tmpString);
   
      
}

void sendTemp(){
//  result += "BEGIN";
//  result += "ROM";
//  result += "END#";
}

void readTempDHT(void){
  DHT_1.read22(PIN_DHT_1);  
}

void switchScreen(void){
  if(digitalRead(PIN_BUTTON) == HIGH){
    screen = (screen == SCREEN_DS18B20 ? SCREEN_DHT : SCREEN_DS18B20);
    printTemp();
  }
}

boolean timeElapsed(unsigned long *previousMillis, unsigned int interval) 
{
 currentMillis = millis();
 diff = currentMillis - *previousMillis;
 
 if(diff < 0){
   // millis() returns the number of milliseconds since the Arduino board
   // began running the current program. This number will overflow (go back to zero),
   // after approximately 50 days.
   // After overflow, diff will be negative so we have to set up previousMillis.
   *previousMillis = currentMillis;
 }
 else {
   if(diff >= interval)
   {
     *previousMillis = currentMillis;
     return true;
   }
   else {
     return false;
   }
 }
}
