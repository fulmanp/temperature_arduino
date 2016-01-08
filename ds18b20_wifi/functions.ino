boolean sendATCommand(String ATCommand, String replay, int timeToWaitForComplete){
  
  Serial.print(ATCommand);
  Serial.print("\r\n");
  
  String response = "";
  char c;
    
  long int time = millis();
    
  while( (time+timeToWaitForComplete) > millis())
  {
    while(Serial.available() > 0){
      c = Serial.read(); 
      response += c;
    }
    if(response.indexOf(replay)!=-1)
      return true;
  }
  
  return false;
}

boolean commandAT_AT(){
  return sendATCommand("AT", "OK", AT_COMMAND_DELAY);
}

boolean commandAT_CWMODE(){
  return sendATCommand("AT+CWMODE=1", "OK", AT_COMMAND_DELAY);
}

boolean commandAT_CWJAP(char* ssid, char* password){
  char command[256];
  sprintf(command, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
  return sendATCommand(command, "OK", WIFI_AP_CONNECT);
}

boolean commandAT_CIPSTART(char* server){
  char command[256];
  sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",80", server);
  return sendATCommand(command, "OK", WIFI_TCP_CONNECT);
}

boolean commandAT_CIPSEND(char* method, char* uri, char* server){
  int len = 6;
  char command[256];
  char requestStartLine[256];   
  char requestHeaderHost[256];
  boolean result;
  
  sprintf(requestStartLine, "%s %s HTTP/1.1", method, uri);
  len += strlen(requestStartLine);
  sprintf(requestHeaderHost, "Host: %s", server);
  len += strlen(requestHeaderHost);
  sprintf(command, "AT+CIPSEND=%d", len);
  if(result=sendATCommand(command, "OK", AT_COMMAND_DELAY)){  
    Serial.write(requestStartLine);
    Serial.write(13);
    Serial.write(10);
    Serial.write(requestHeaderHost);
    Serial.write(13);
    Serial.write(10);
    Serial.write("");
    Serial.write(13);
    Serial.write(10);
  }  
  return result; 
}

boolean commandAT_CIPCLOSE(){
  return sendATCommand("AT+CIPCLOSE", "OK", AT_COMMAND_DELAY);
}

boolean commandAT_CWQAP(){
  return sendATCommand("AT+CWQAP", "OK", AT_COMMAND_DELAY);
}


boolean sendMessage(char*  ssid, char*  password, char* server, char* method, char* uri, int maxTrials){
  while(_sendMessage(ssid, password, server, method, uri) && maxTrials){
    maxTrials--;
  }
  if(maxTrials)
    return true;
  return false;
}

int _sendMessage(char*  ssid, char*  password, char* server, char* method, char* uri){
  int maxTrialsAT = 5;
  
  while(!commandAT_AT() && maxTrialsAT){
    delay(1000);
    maxTrialsAT--;
  }
  if(maxTrialsAT){
    if(commandAT_CWMODE()){
      if(commandAT_CWJAP(ssid, password)){       
        if(commandAT_CIPSTART(server)){          
          if(commandAT_CIPSEND(method, uri, server)){                
            if(!commandAT_CIPCLOSE())
              return 6;
            if(!commandAT_CWQAP())
              return 7;
          }// CIPSEND
          else {
            return 5;
          } 
        }// CIPSTART
        else {
          return 4;
        } 
      }// CWJAP
      else {
        return 3;
      } 
    }// CWMODE
    else {
      return 2;
    }    
  }// maxTrials
  else {
    return 1;
  }
  
  return 0;
}


