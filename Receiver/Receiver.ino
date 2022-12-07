#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <FirebaseESP32.h>
#include <WiFi.h>


// WiFi configuration
#define WIFI_SSID "IphoneXr"                                          // input your home or public wifi name
#define WIFI_PASSWORD "11111111"

// Firebase configuration
#define FIREBASE_HOST "doan-3f0dd-default-rtdb.firebaseio.com"      // the project name address from firebase id
#define FIREBASE_AUTH "qzKiNHBVt3BLsLWKJGZC9Hpu2VN7dlvWytY5AjXh"    // the secret key generated from firebase

FirebaseData dulieufirebase;

StaticJsonDocument<200> doc;

const byte rxPin = 16;
const byte txPin = 17;
// Set up a new SoftwareSerial object
SoftwareSerial myLoRa(rxPin, txPin);

String data = "";

unsigned long previousMillis = 0;
unsigned long interval = 15000;
unsigned long times=millis();

char dataRcvd[200];
int pos = 0;

void setup()
{
  Serial.begin(9600);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
     delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  // Begin Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi("true");

  setData("Mayphunsuong", "false");
  setData("Maysuoi", "false");
  setData("Mayquat", "false");
  setData("Loa", "false");
  // setData("Keo_man_che_len","false");
  // setData("Keo_man_che_xuong","false");  
  setData("AutoMode", "false");
  setData("GH_NhietDo_LOW", "28");
  setData("GH_NhietDo_High", "30");
  setData("GH_DoAm_High", "80");
  setData("GH_DoAm_LOW", "60");
  setData("GH_AS_High", "50"); 
  setData("GH_AS_Low", "30"); 
  setData("Giobatloa", "false"); 
  setData("Giotatloa", "false");

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
    
  // Set the baud rate for the LoRa module
  myLoRa.begin(9600);

  // Setting Time
}

void loop()
{
  //ds1307();
  dk_thietbi();

  while (myLoRa.available() > 0){
    char inByte = myLoRa.read();
    if (inByte == '\n'){
      // process the char array
      // Deserialize the JSON document
      char json[pos-1];
      for (int i = 0 ; i < pos; i++){
        Serial.print(dataRcvd[i]);
        json[i] = dataRcvd[i];
      }
      DeserializationError error = deserializeJson(doc, json);
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      // Fetch values.
      int t = doc["t"];
      int h = doc["h"];
      int lux = doc["lux"];
      // Print values.
      Serial.println("--------------------\nReceived data:");
      Serial.print("t = "); Serial.println(t);
      Serial.print("h = "); Serial.println(h);
      Serial.print("lux = "); Serial.println(lux);

      // Send received data to Firebase
      Firebase.setInt(dulieufirebase, "Nhiet_Do", t);
      Firebase.setInt(dulieufirebase, "Do_Am", h);
      Firebase.setInt(dulieufirebase, "CuongDoAnhSang", lux);

      // khoi tao lai char array
      pos = 0;
    } else {
      dataRcvd[pos] = inByte;
      pos++;
    }
  }
}

void setData(String url, String data1){
  if(Firebase.setString(dulieufirebase, url, data1)){
    //Success
    Serial.println("Set string data success");
  } else {
    //Failed?, get the error reason from firebaseData
    Serial.print("Error in setString, ");
    Serial.println(dulieufirebase.errorReason());
  }
}

String getData(String url){
  Firebase.getString(dulieufirebase, url);
  String data1 = dulieufirebase.stringData();
  return data1;
}

String xuli(String str)
{
  int len = str.length();
  for(int i=1; i<3; i++)
  {
    str[len-i] = '\0';
  }
  int len1 = str.length();
  for(int i=0; i < len1; i++)
  {   
    if(i < len1-2){
    str[i] = str[i+2];
    } else {
        str[i] = '\0';
    }
  }
  Serial.println(str);
  return str;
}
 
int xuli1(String a)
{
  int c = xuli(a).length()-3;
  char array_1[c];
  xuli(a).toCharArray(array_1,c);
  int d = atoi(array_1);
  Serial.println(array_1);
  Serial.println(d);
  return d;
}

/*
void ds1307(){
  hours = RTC.getHours();
  Serial.print("hours is: ");

  Serial.println(hours);
}
*/

void dk_thietbi()
{
  unsigned long currentMillis = millis();
  if(currentMillis - times > 1000){
    String AutoMode = getData("AutoMode");

    if(AutoMode == "false"){
      //lcd.setCursor(0,1); lcd.print("Auto:OFF");
      for(int i=0; i<100; ++i);
      String Mayphunsuong = getData("Mayphunsuong");
      if (Mayphunsuong == "true"){
        myLoRa.write(0x03);
        //digitalWrite(mayphunsuongPin, 1);
        // lcd.setCursor(0,3);
        // lcd.print("Suong: ON");
      } else if(Mayphunsuong == "false"){
        myLoRa.write(0x04);
        // digitalWrite(mayphunsuongPin, 0);
        // lcd.setCursor(0,3);
        // lcd.print("Suong:OFF");
      } else {
        Serial.println("Wrong Credential! Please send ON/OFF");
        // lcd.setCursor(0,3);lcd.print("Suong:OFF");
      }

      for(int i=0; i<100; ++i);
      String Maysuoi = getData("Maysuoi");
      if(Maysuoi == "true") {
        myLoRa.write(0x05);
        // digitalWrite(maysuoiPin, 1);
        // lcd.setCursor(10,3); lcd.print("Suoi: ON");
      } else if(Maysuoi == "false") {
        myLoRa.write(0x06);
        // digitalWrite(maysuoiPin,0);
        // lcd.setCursor(10,3); lcd.print("Suoi:OFF");
      } else {
      Serial.println("Wrong Credential! Please send ON/OFF");
      // lcd.setCursor(10,3); lcd.print("Suoi:OFF");
      }
      for(int i=0; i<100; ++i);

      String Mayquat = getData("Mayquat");
      if(Mayquat == "true") {
        myLoRa.write(0x07);
        // digitalWrite(mayquatPin,1);
        // lcd.setCursor(10,2); lcd.print("Quat: ON");
      } else if(Mayquat == "false") {
        myLoRa.write(0x08);
        // digitalWrite(mayquatPin,0);
        // lcd.setCursor(10,2); lcd.print("Quat:OFF"); 
      } else { 
        Serial.println("Wrong Credential! Please send ON/OFF");
        // lcd.setCursor(10,2); lcd.print("Quat:OFF");
      }

      for(int i=0; i<100; ++i);
      String Loa = getData("Loa");
      if(Loa == "true") {
        myLoRa.write(0x09);
        // digitalWrite(loaPin,1);
        // lcd.setCursor(0,2); lcd.print("Loa: ON");
      } else if(Loa == "false") {
        myLoRa.write(0x0A);
        // digitalWrite(loaPin,0);
        // lcd.setCursor(0,2); lcd.print("Loa:OFF");
      }  
      else {
        Serial.println("Wrong Credential! Please send ON/OFF");
        myLoRa.write(0x0A);
        // digitalWrite(loaPin,0); lcd.setCursor(0,2); lcd.print("Loa:OFF");
      }   
    /*for(int i=0; i<1000; ++i);
    String Donglen = getData("Keo_man_che_len");
    String Haxuong = getData("Keo_man_che_xuong");
    if(Donglen == "true" & Haxuong == "false") {digitalWrite(keomanchelen,1); digitalWrite(keomanchexuong,0);}
    else if(Donglen == "false" & Haxuong == "false") {digitalWrite(keomanchelen,0);digitalWrite(keomanchexuong,0); } 
    else if(Donglen == "false" & Haxuong == "true") {digitalWrite(keomanchelen,0); digitalWrite(keomanchexuong,1);}
    else { Serial.println("Wrong Credential! Please send ON/OFF");}*/
    }/*
    else if(AutoMode == "true") 
    {
      // lcd.setCursor(0,1); lcd.print("Auto: ON");
      for(int i=0; i<100; ++i);
      String GH_DoAm_High = getData("GH_DoAm_High");
      String GH_DoAm_Low = getData("GH_DoAm_LOW");
      if(DoAm > xuli1(GH_DoAm_Low) && DoAm < xuli1(GH_DoAm_High))
      {
        for(int i=0; i<1000; ++i);
        String GH_NhietDo_Low = getData("GH_NhietDo_LOW");
        String GH_NhietDo_High = getData("GH_NhietDo_High");
        if(NhietDo > xuli1(GH_NhietDo_Low) && NhietDo < xuli1(GH_NhietDo_High))
        {
          // digitalWrite(mayphunsuongPin,0); lcd.setCursor(0,3); lcd.print("Suong:OFF");
          // digitalWrite(maysuoiPin,0); lcd.setCursor(10,3); lcd.print("Suoi:OFF");
          // digitalWrite(mayquatPin,0); lcd.setCursor(10,2); lcd.print("Quat:OFF");
          myLoRa.write(0x04); delay(100);
          myLoRa.write(0x06); delay(100);
          myLoRa.write(0x08); delay(100);
          Serial.print("Nhiet do on dinh"); Serial.println();
        }
        else if(NhietDo >= xuli1(GH_NhietDo_High))
        {
          // digitalWrite(mayphunsuongPin,1); lcd.setCursor(0,3); lcd.print("Suong: ON");
          // digitalWrite(maysuoiPin,0); lcd.setCursor(10,3); lcd.print("Suoi:OFF");
          // digitalWrite(mayquatPin,0);lcd.setCursor(10,2); lcd.print("Quat:OFF");
          myLoRa.write(0x03); delay(100);
          myLoRa.write(0x06); delay(100);
          myLoRa.write(0x08); delay(100);
          Serial.print("Nhiet do cao"); Serial.println();     
        }
        else
        {
          // digitalWrite(mayphunsuongPin,0); lcd.setCursor(0,3); lcd.print("Suong:OFF");
          // digitalWrite(maysuoiPin,1); lcd.setCursor(10,3); lcd.print("Suoi: ON");
          // digitalWrite(mayquatPin,0);lcd.setCursor(10,2); lcd.print("Quat:OFF");
          myLoRa.write(0x04); delay(100);
          myLoRa.write(0x05); delay(100);
          myLoRa.write(0x08); delay(100);
          Serial.print("Nhiet do thap"); Serial.println();
        }
      }
      else if(DoAm >= xuli1(GH_DoAm_High))
      {
        // digitalWrite(mayphunsuongPin,0); lcd.setCursor(0,3); lcd.print("Suong:OFF");
        // digitalWrite(maysuoiPin,0); lcd.setCursor(10,3); lcd.print("Suoi:OFF");
        // digitalWrite(mayquatPin,1);lcd.setCursor(10,2); lcd.print("Quat: ON");
        myLoRa.write(0x04); delay(100);
        myLoRa.write(0x06); delay(100);
        myLoRa.write(0x07); delay(100);
        Serial.print("Do Am cao"); Serial.println();
      }
      else
      {
        // digitalWrite(mayphunsuongPin,1); lcd.setCursor(0,3); lcd.print("Suong: ON");
        // digitalWrite(maysuoiPin,0); lcd.setCursor(10,3); lcd.print("Suoi:OFF");
        // digitalWrite(mayquatPin,0);lcd.setCursor(10,2); lcd.print("Quat:OFF");  
        myLoRa.write(0x03); delay(100);
        myLoRa.write(0x06); delay(100);
        myLoRa.write(0x08); delay(100);
        Serial.print("Do am thap"); Serial.println();       
      }

      for(int i=0; i<1000; ++i);
      String Hen_gio_bat_loa = getData("Giobatloa");
      String Hen_gio_tat_loa = getData("Giotatloa");
      if (xuli1(Hen_gio_bat_loa) > xuli1(Hen_gio_tat_loa))
      {
        if(hours >= xuli1(Hen_gio_tat_loa) && hours < xuli1(Hen_gio_bat_loa)) 
        {
          // digitalWrite(loaPin, 0); lcd.setCursor(0,2); lcd.print("Loa: OFF");    
          myLoRa.write(0x0A);
        }
        else
        {
          // digitalWrite(loaPin, 1); lcd.setCursor(0,2); lcd.print("Loa: ON");
          myLoRa.write(0x09);
        }
      }
      else
      {
        if(hours >= xuli1(Hen_gio_bat_loa) & hours < xuli1(Hen_gio_tat_loa))
        {
          // digitalWrite(loaPin,1); lcd.setCursor(0,2); lcd.print("Loa: ON");
          myLoRa.write(0x09);
        }
        else
        {
          // digitalWrite(loaPin,0); lcd.setCursor(0,2); lcd.print("Loa:OFF");
          myLoRa.write(0x0A);
        }
      }
    }*/
    times = currentMillis;
  }
}