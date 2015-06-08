#include <OneWire.h>
#include <LiquidCrystal.h>
#include <dht.h>

#define BUTTON_PIN 3
#define DATA_LED_PIN  4
#define DHT_PIN 5

dht DHT;
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary)
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); //lcd(5, 6, 9, 10, 11, 12);

int counter = 0;
bool flag = false;
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius, fahrenheit;
String result = "";
String dhtResult = "";
char val;

void setup(void) {
  //Serial.begin(9600);// default for terminal
  Serial.begin(19200);// for my bt module
  pinMode(DATA_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(1, button, CHANGE);
  // Choose screen size - 20x4
  lcd.begin(20, 4); //lcd.begin(16, 2); 
  lcd.clear();
  // Display
  lcd.print("Temp & Humid");
  delay(1000);
  lcd.clear();
}

void loop(void) {
//  int state = digitalRead(BUTTON_PIN);
//  
//  if(state == HIGH){
//    flag = !flag;  // button pressed
//    //digitalWrite(DATA_LED_PIN, HIGH);
//  }
  
  //int flag;
  //delay(1000);
  //lcd.clear();
  if(Serial.available() <= 0){
    if(flag == false){  // Read from DHT21
      readDHT();
    }
    else if(flag == true){
      //lcd.clear();
      read();
    }
  }

  //lcd.clear();
  if(Serial.available() > 0){    
   lcd.clear(); 
    val = Serial.read();
    //Serial.println(">");   
    //Serial.println(val);
    //Serial.println("<");
    //flag=0;
    
    if (val == '1' || val == '0'){
      if (val == '1'){
        Serial.println("1 received");
        digitalWrite(DATA_LED_PIN, HIGH);
        delay(100);
        digitalWrite(DATA_LED_PIN, LOW);
      }
      
      read();
   }
 }
}
