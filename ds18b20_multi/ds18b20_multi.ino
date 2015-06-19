#include <OneWire.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <ArduinoJson.h>

#define PIN_BUTTON 3
#define PIN_DATA_LED  4
#define PIN_DHT_1 5
#define INTERVAL_TEMP_READ 1000*60
#define SCREEN_DS18B20 0
#define SCREEN_DHT 1
#define MAX_DS18B20 4
// Saying the truth, MAX_DHT is the number of DHT sensors which now are used.
#define MAX_DHT 1
#define DISP_TYPE_4_20 1

dht DHT_1;

OneWire  ds(2);                         // on pin 2 (a 4.7K resistor is necessary)
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); //lcd(5, 6, 9, 10, 11, 12);

typedef struct 
{
  float celsius;
  float fahrenheit;
  String rom;    // ROM as readable string
  char romC[17]; // ROM as readable sequence of chars; last one is for null char ('\0')
  byte addr[8];  // ROM as array of bytes
  bool valid;
} ds18b20Sensor;

int counter = 0; // for DS18B20; iterator which counts the number of sensors for current temp read
ds18b20Sensor ds18b20Sensors[MAX_DS18B20];

int screen = SCREEN_DS18B20; // indicates what screen should be printed: for DS18B20 or DHT sensor(s)
byte i, j;        // for loop iterator
byte present = 0; // for DS18B20 - taken form example, not sure if we need this
byte type_s;      // for DS18B20 ???
byte data[12];    // for DS18B20
byte *addr;       // for DS18B20
String tmpString = "";  // variable used to prepare string to print on lcd
char valFromSerial;     // variable used to keep value send by serial (from BT)

unsigned long lastMillisTemperatureRead = 0;
unsigned long tick = 0; // Counts how many times temperature was read
unsigned long d, h, m;  // variables used to print "time"

// Local variable used by timeElapsed() function
unsigned long currentMillis;
unsigned long diff;

void setup(void) {
  Serial.begin(19200); // 9600 is default for terminal. For my BT module use 19200
  pinMode(PIN_DATA_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  attachInterrupt(1, switchScreen, CHANGE);  
  initDisplay(DISP_TYPE_4_20);
  readTemp();
  printTemp(DISP_TYPE_4_20);
}

void loop(void) {
  if(timeElapsed(&lastMillisTemperatureRead, INTERVAL_TEMP_READ)){
    tick++;
    readTemp();
    printTemp(DISP_TYPE_4_20);
  } else {
    if(Serial.available() > 0){    
      valFromSerial = Serial.read();  
      if (valFromSerial == '1' || valFromSerial == '0'){
        if (valFromSerial == '1'){
          digitalWrite(PIN_DATA_LED, HIGH);
          delay(100);
          digitalWrite(PIN_DATA_LED, LOW);
        }      
        readTemp();
        printTemp(DISP_TYPE_4_20);
        sendTemp();
     }
   }
  }  
}

void initDisplay(int dispType){
  if (dispType == DISP_TYPE_4_20){
    lcd.begin(20, 4); // Choose screen size - 20x4
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("      Welcome!      ");
    lcd.setCursor(0,1);
    lcd.print("    Temp & Humid    ");
    lcd.setCursor(0,2);
    lcd.print("      Station       ");
    delay(2000);
    lcd.clear();
  }
}

void readTemp(){
  readTempDS18B20();
  readTempDHT();
}
