#include <SoftwareSerial.h>

const byte rxPin = 16;
const byte txPin = 17;
// Set up a new SoftwareSerial object
SoftwareSerial myLoRa(rxPin, txPin);

String data = "";

unsigned long previousMillis = 0;
unsigned long interval = 15000;

char dataRcvd[200];

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
    int numOfBytes = myLoRa.available();
    Serial.println("Number of incoming bytes = %d", numOfBytes);
  }
}
