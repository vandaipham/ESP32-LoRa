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
}

void loop()
{
  if (myLoRa.available() > 0){
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