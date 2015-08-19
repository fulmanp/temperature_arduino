#include <OneWire.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <ArduinoJson.h>

// ==================================== BEGIN
// Set correct values for your project
#define PIN_BUTTON    3
#define PIN_DATA_LED 13
#define PIN_DHT_1     4
#define PIN_DS18B20   2
#define LCD_RS        7
#define LCD_ENABLE    8
#define LCD_D4        9
#define LCD_D5       10
#define LCD_D6       11
#define LCD_D7       12
// ==================================== END

#define INTERVAL_TEMP_READ 1000*60
#define SCREEN_DS18B20 0
#define SCREEN_DHT_1 1
#define SCREEN_TICKS 2
// If you change MAX_XXX remember to set up correct values in counterS array in sendTemp function
#define MAX_DS18B20 4
// Saying the truth, MAX_DHT is the number of DHT sensors which now are used.
#define MAX_DHT 1
#define DISP_TYPE_4_20 1
#define DISP_TYPE_2_16 2
#define CURRENT_DISP DISP_TYPE_4_20
#define ICON_THERMOMETER 1
#define ICON_DROPLET 2
#define ICON_ROM 3

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

int screen = SCREEN_DS18B20; // indicates which screen should be printed: with DS18B20 info or DHT sensor(s) info or...
int current_ds18b20 = 0;
int number_of_ds18b20 = MAX_DS18B20; // initial value; this value is owerwritten in readTempDS18B20 function
int current_dht = 0;
int number_of_dht = MAX_DHT;
byte i, j, k;     // for loop iterator
int rows;         // number of rows for current display
byte present = 0; // for DS18B20 - taken form example, not sure if we need this
byte type_s;      // for DS18B20 ???
byte data[12];    // for DS18B20
byte *addr;       // for DS18B20
String tmpString = "";  // variable used to prepare string to print on lcd
char valFromSerial;     // variable used to keep value send by serial (from BT)

unsigned long lastMillisTemperatureRead = 0;
unsigned long tick = 0; // Counts how many times temperature was read; I assume that 1 tick = 60 seconds
unsigned long d, h, m;  // variables used to print "time"

// Local variable used by timeElapsed() function
unsigned long currentMillis;
unsigned long diff;

/*
byte icon_thermometer[8] = //icon for thermometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};
*/
byte icon_thermometer[8] = //icon for thermometer
{
    B00100,
    B00110,
    B00100,
    B11011,
    B00100,
    B00110,
    B00100,
    B00110
};

byte icon_droplet[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

byte icon_rom[8] = //icon for rom chip
{
    B11111,
    B01110,
    B11111,
    B01110,
    B11111,
    B01110,
    B11111,
    B00000,
};

dht DHT_1;

OneWire  ds(PIN_DS18B20);

// For "LiquidCrystal lcd" documentation see:
// https://www.arduino.cc/en/Reference/LiquidCrystalConstructor
// LiquidCrystal(rs, enable, d4, d5, d6, d7) 
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7); 

void setup(void) {
  Serial.begin(9600); // 9600 is default for terminal. For my BT module use 19200
  pinMode(PIN_DATA_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  attachInterrupt(1, switchScreen, CHANGE);  
  lcd.createChar(ICON_THERMOMETER, icon_thermometer);
  lcd.createChar(ICON_DROPLET, icon_droplet);
  lcd.createChar(ICON_ROM, icon_rom);
  initDisplay(CURRENT_DISP);
  readTemp();
  updateScreen(CURRENT_DISP);
}

void loop(void) {
  if(timeElapsed(&lastMillisTemperatureRead, INTERVAL_TEMP_READ)){
    tick++;
    readTemp();
    updateScreen(CURRENT_DISP);
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
        updateScreen(CURRENT_DISP);
        sendTemp();
     }
   }
  }  
}

void initDisplay(int dispType){
  if (dispType == DISP_TYPE_4_20){
    lcd.begin(20, 4);
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("      Welcome!      ");
    lcd.setCursor(0,1);
    lcd.print("    Temp & Humid    ");
    lcd.setCursor(0,2);
    lcd.print("      Station       ");
    delay(2000);
    lcd.clear();
  } else if (dispType == DISP_TYPE_2_16){
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Temp & Humid  ");
    lcd.setCursor(0,1);
    lcd.print("    Station     ");
    delay(2000);
    lcd.clear();
  }
}

void readTemp(){
  readTempDS18B20();
  readTempDHT();
}
