void issLocOLEDDisplay() {
  display.clear();
  display.drawString(0,0,"The ISS is currently at: ");
 char temp[15];
 sprintf(temp, "%d.%02d,%d.%02d",(int)isslat,abs((int)(isslat*100)%100),(int)isslon,abs((int)(isslon*100)%100));
 display.drawString(25,15,temp);
 char temp1[30];
 sprintf(temp1, "ISS is about %d miles", distance);
 display.drawString(0,27,temp1);
 display.drawString(30,38, "from you.");
 display.drawString(12,51, "And moving fast!!");
 display.display();
}

void issLocSerialDisplay() {
  Serial.print("The ISS is currently at ");
  Serial.print(isslat, 4); Serial.print(","); Serial.println(isslon,4);
  Serial.print("The ISS is about "); Serial.print(distance); Serial.println(" miles from you now.\nAnd moving fast!");
    
}

void getJson(String url) {
 
 if (wifiMulti.run() == WL_CONNECTED) { //Check WiFi connection status
     HTTPClient http;  //Declare an object of class HTTPClient
     http.begin(url);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
     if (httpCode > 0) { //Check the returning code
       payload = http.getString();   //Get the request response payload
     
    }
 
    http.end();   //Close connection
 
  }

    
}

void decodeLocJson() {
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  isslat=root["iss_position"]["latitude"];
  isslon=root["iss_position"]["longitude"];
}

void getDistance() {
  float theta, dist, miles;
  theta = mylon - isslon;
  dist = sin(deg2rad(mylat)) * sin(deg2rad(isslat)) + cos(deg2rad(mylat)) * cos(deg2rad(isslat)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  miles = dist * 60 * 1.1515;
  distance = miles;
}

float deg2rad(float n) {
  float radian = (n * 71)/4068;
  return radian;
}

float rad2deg(float n) {
  float degree = (n*4068)/71;
  return degree;
}

void decodePeopleJson() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  number = root["number"];
  if (number > 10) {number = 10;}
  for (int i=0;i<number; i++){
    
    const char* temp1 = root["people"][i]["name"];
    const char* temp2 = root["people"][i]["craft"];
    name[i] = (String)temp1;
    craft[i] = (String)temp2;
  }
 }

 void displayPeopleSerial() {
  Serial.print("There are "); Serial.print(number); Serial.println(" people in space right now.");
  for (int i=0;i<number; i++) {
    Serial.print(name[i]);Serial.print(" on board ");Serial.println(craft[i]);
  }
 }

 void displayPeopleOLED() {
  display.clear();
  char temp2[50];
  sprintf(temp2, "%d people are in space.", number);
  display.drawString(0,0,temp2);

 if (number > 5) {number = 5;} //Display the 1st 5 Astros on OLED 
 for (int i=0;i<number; i++) {
 display.drawString(0,10*(i+1),name[i] + ", " + craft[i]);
 }
  
 display.display();
}

void decodePassJson() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  count = root["request"]["passes"];
  
  if (count > 5) {count = 5;}
  for (int i=0;i<count; i++){
    
    unsigned int tempEpoch = root["response"][i]["risetime"];
    risetime[i] = convertEpoch(tempEpoch);
    duration[i] = root["response"][i]["duration"];
    duration[i] = duration[i] / 60;
      }
 }

 String convertEpoch(unsigned int epoch) {
  int h = hour(epoch)-6;
  int m = minute(epoch);
  int d = day(epoch);
  int mn = month(epoch);
  int y = year(epoch);
   char temp[100];
  sprintf(temp, "RT: %d/%d %d:%d MST",mn,d,h,m);
  return (String)temp;
 }

void displayPassSerial() {
  Serial.println("Pass Prediction");
  for (int i=0;i<count; i++) {
    Serial.print(risetime[i]); Serial.print(" [");Serial.print(duration[i]);Serial.println(" mins.]");
  }
}

 void displayPassOLED() {
  display.clear();
  display.drawString(0,0,"Pass Prediction");

 for (int i=0;i<count; i++) {
 display.drawString(0,10*(i+1),risetime[i] + " [" + (String)duration[i]+" m]");
 }
  
 display.display();
}
