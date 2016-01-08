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
