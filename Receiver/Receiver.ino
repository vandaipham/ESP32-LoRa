#include <SoftwareSerial.h>
#include <ArduinoJson.h>


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

      // khoi tao lai char array
      pos = 0;
    } else {
      dataRcvd[pos] = inByte;
      pos++;
    }
  }
}
