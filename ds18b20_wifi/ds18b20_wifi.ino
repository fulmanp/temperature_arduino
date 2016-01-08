#define AT_COMMAND_DELAY 2000
#define WIFI_AP_CONNECT 20000
#define WIFI_TCP_CONNECT 5000

void setup(void) {
  int maxTrialsAT = 10;
  
  delay(3000);
  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  while(!sendATCommand("AT", "OK", AT_COMMAND_DELAY) && maxTrialsAT){
    maxTrialsAT--;
    delay(1000);    
  }
  
  sendATCommand("AT+CWMODE=1", "OK", AT_COMMAND_DELAY);
  sendATCommand("AT+CWJAP=\"FunBox-FC08\",\"61EA91C5CA152A6E1793ADA434\"", "OK", WIFI_AP_CONNECT);
  sendATCommand("AT+CIPSTART=\"TCP\",\"fulmanski.pl\",80", "OK", WIFI_TCP_CONNECT);
  sendATCommand("AT+CIPSEND=66", "OK", AT_COMMAND_DELAY);
  Serial.write("GET /php/temperature/test.php?a=1 HTTP/1.1");
  Serial.write(13);
  Serial.write(10);
  Serial.write("Host: fulmanski.pl");
  Serial.write(13);
  Serial.write(10);
  Serial.write("");
  Serial.write(13);
  Serial.write(10);
  sendATCommand("AT+CIPCLOSE", "OK", AT_COMMAND_DELAY);
  sendATCommand("AT+CWQAP", "OK", AT_COMMAND_DELAY);         
}

void loop(void) {

}

