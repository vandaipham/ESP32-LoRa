#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoJson.h>


// DHT configuration
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int NhietDo, DoAm;

// BH1750 Configuration
BH1750 lightMeter(0x23);
uint16_t lux;

// LoRa port configuration
const byte rxPin = 16;
const byte txPin = 17;
SoftwareSerial myLoRa(rxPin, txPin);

// Allocate the JSON document
// Inside the brackets, 200 is the RAM allocated to this document.
// Don't forget to change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<200> doc;

String data = "";

unsigned long previousMillis = 0;
unsigned long interval = 10000;


void setup()
{
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // Set the baud rate for the LoRa module
  myLoRa.begin(9600);

  // Start DHT
  dht.begin();

  // Start BH1750
  Wire.begin(19,21);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    /* The Arduino executes this code once every second
    *  (interval = 1000 (ms) = 1 second).
    */
    // Send Hello world over LoRa
    //Serial.println("Start sending.....");
    //myLoRa.write("Hello world\n");
    //Serial.println("Hello world was sent\n");
    //delay(1000);
    // Send data over LoRa
    Serial.println("Sending data...");
    sendData();
    Serial.println("Data were sent!!\n");
    previousMillis = currentMillis;
  }
/*
  while (myLoRa.available() > 0){
    char dataRcvd = myLoRa.read();
    if (dataRcvd == '\n'){
      Serial.println("End of message");
    } else
      data += dataRcvd;
  }
  if (data != ""){
    Serial.println(data);
    data = "";
  }
*/
}

void sendData(){
  read_DHT11();
  read_BH1750();
  // Add values in the document
  doc["h"] = DoAm;
  doc["t"] = NhietDo;
  doc["lux"] = lux;
  // Generate the minified JSON and send it to the Serial port.
  serializeJson(doc, Serial);
  // The above line prints:
  // {"h": DoAm,"t":NhietDo,"lux":lux}
  Serial.println();

  // Send Json data over LoRa
  serializeJson(doc, myLoRa);
  // Send EOL
  myLoRa.write("\r\n");
}

void read_DHT11()
{
  DoAm = dht.readHumidity();
  // Read temperature as Celsius (the default)
  NhietDo = dht.readTemperature();
  if (isnan(NhietDo) || isnan(DoAm)) {
    Serial.println("Failed to read from DHT");
  }
  Serial.print(NhietDo);
  Serial.print(" oC, ");
  Serial.print(DoAm);
  Serial.println(" %\n");
  
//  Firebase.setInt(dulieufirebase, "Nhiet_Do", NhietDo);
//  Firebase.setInt(dulieufirebase, "Do_Am", DoAm);

}

void read_BH1750(){
  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx\n");
  //Firebase.setInt(dulieufirebase, "CuongDoAnhSang", lux);
}
