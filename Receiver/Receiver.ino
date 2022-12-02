#include <SoftwareSerial.h>

const byte rxPin = 16;
const byte txPin = 17;
// Set up a new SoftwareSerial object
SoftwareSerial myLoRa(rxPin, txPin);

String data = "";

unsigned long previousMillis = 0;
unsigned long interval = 15000;


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
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    /* The Arduino executes this code once every second
    *  (interval = 1000 (ms) = 1 second).
    */
    // Send data over LoRa
    myLoRa.write("Hello world from Node 2");
    
    previousMillis = currentMillis;
  }

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
}
