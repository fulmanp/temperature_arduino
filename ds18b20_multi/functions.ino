//byte i;
//byte present = 0;
//byte type_s;
//byte data[12];
//byte addr[8];
//float celsius, fahrenheit;
//String result = "";
//String dhtResult = "";
//char val;

void read(){  
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
        
        //lcd.clear();
        display();
      }
      
      if(counter >= 2)
        counter = 0;
      
      result += "END#";    
            
      //if(flag == 0){
        Serial.println(result);
        //flag = 1;
      //}
      
      delay(2000);
      //lcd.clear();
      result = "";
}

void display(void){
  lcd.setCursor(0, 0);
  lcd.print("DS18b20");
  lcd.setCursor(0, counter % 3 + 1);
  lcd.print(celsius);
  counter += 1;
}

void readDHT(){
  //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DHT 21");
      lcd.setCursor(0, 1);
      DHT.read22(DHT_PIN);
      dhtResult += "Temp: ";
      dhtResult += DHT.temperature;
      lcd.print(dhtResult);
      dhtResult = "";
      //delay(1000);
      lcd.setCursor(0, 2);
      dhtResult += "Humidity: ";
      dhtResult += DHT.humidity;
      lcd.print(dhtResult);
      delay(5000);
      dhtResult = "";
}

void button(){
  if(digitalRead(BUTTON_PIN) == HIGH){
    flag = !flag;  // button pressed
    lcd.clear();
    //digitalWrite(DATA_LED_PIN, HIGH);
  }
}
