#define AT_COMMAND_DELAY 2000
#define WIFI_AP_CONNECT 20000
#define WIFI_TCP_CONNECT 5000
#define MAX_SEND_TRIALS 3

void setup(void) {
  
  char *ssid = "FunBox-FC08";
  char *password = "61EA91C5CA152A6E1793ADA434";
  char *server = "fulmanski.pl";
  char *method = "GET";
  char *uri = "/php/temperature/test.php?a=1";
  
  delay(3000);
  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  sendMessage(ssid, password, server, method, uri, MAX_SEND_TRIALS);          
}

void loop(void) {

}

