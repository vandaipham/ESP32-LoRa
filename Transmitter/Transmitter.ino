#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>


// LCD configuration
LiquidCrystal_I2C lcd(0x27, 20, 4);

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

// GPIO
int loaPin = 33;
int mayquatPin= 32;
int mayphunsuongPin = 27;
int maysuoiPin = 26;

// AutoMode state
int autoMode = 0;
const int GH_NhietDo_LOW = 28;
const int GH_NhietDo_High = 30;
const int GH_DoAm_High = 80;
const int GH_DoAm_LOW = 60;
const int GH_AS_High = 50;
const int GH_AS_Low = 30;

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
  delay(3000);
  // Begin LCD
  Wire.begin(19,21);
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("ND:");
  lcd.setCursor(10,0);
  lcd.print("DA:");
  lcd.setCursor(10,1);
  lcd.print("AS:");
  lcd.setCursor(0,1);
  lcd.print("Auto:OFF");
  lcd.setCursor(0,2);
  lcd.print("Loa:OFF");
  lcd.setCursor(10,2);
  lcd.print("Quat:OFF");
  lcd.setCursor(0,3);
  lcd.print("Suong:OFF");
  lcd.setCursor(10,3);
  lcd.print("Suoi:OFF");

  // GPIO setting
  pinMode(loaPin,OUTPUT); digitalWrite(loaPin,0);
  pinMode(mayquatPin,OUTPUT); digitalWrite(mayquatPin,0);
  pinMode(maysuoiPin,OUTPUT); digitalWrite(maysuoiPin,0);
  pinMode(mayphunsuongPin,OUTPUT); digitalWrite(mayphunsuongPin,0);
}

void loop()
{
  read_DHT11();
  read_BH1750();
  
  unsigned long currentMillis = millis();
  if((currentMillis - previousMillis > interval)) {
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

  while (myLoRa.available() > 0) {
    byte dataRcvd = myLoRa.read();
    Serial.print("Recieved command: "); Serial.println(dataRcvd, HEX); Serial.println();
    switch(dataRcvd) {
      case 0x03:
        digitalWrite(mayphunsuongPin, 1); lcd.setCursor(0, 3); lcd.print("Suong: ON");
        break;
      case 0x04:
        digitalWrite(mayphunsuongPin, 0); lcd.setCursor(0, 3); lcd.print("Suong:OFF");
        break;
      case 0x05:
        digitalWrite(maysuoiPin, 1); lcd.setCursor(10, 3); lcd.print("Suoi: ON");
        break;
      case 0x06:
        digitalWrite(maysuoiPin, 0); lcd.setCursor(10, 3); lcd.print("Suoi: OFF");
        break;
      case 0x07:
        digitalWrite(mayquatPin, 1); lcd.setCursor(10, 2); lcd.print("Quat: ON");
        break;
      case 0x08:
        digitalWrite(mayquatPin, 0); lcd.setCursor(10, 2); lcd.print("Quat:OFF");
        break;
      case 0x09:
        digitalWrite(loaPin, 1); lcd.setCursor(0, 2); lcd.print("Loa: ON");
        break;
      case 0x0A:
        digitalWrite(loaPin, 0); lcd.setCursor(0, 2); lcd.print("Loa: OFF");
        break;
      case 0x01:
        autoMode = 1;
        lcd.setCursor(0,1); lcd.print("Auto: ON");
        break;
      case 0x02:
        autoMode = 0;
        lcd.setCursor(0,1); lcd.print("Auto:OFF");
        break;
    }
  }
  if (autoMode == 1) {
    lcd.setCursor(0,1); lcd.print("Auto: ON");
    autoModeControl();
  }
}

void autoModeControl(){
  if ((DoAm > GH_DoAm_LOW) & (DoAm < GH_DoAm_High)) {
    if ((NhietDo > GH_NhietDo_LOW) & (NhietDo < GH_NhietDo_High)) {
      digitalWrite(mayphunsuongPin, 0); lcd.setCursor(0, 3); lcd.print("Suong:OFF");
      digitalWrite(maysuoiPin, 0); lcd.setCursor(10, 3); lcd.print("Suoi:OFF");
      digitalWrite(mayquatPin, 0);lcd.setCursor(10, 2); lcd.print("Quat:OFF");
      Serial.print("Nhiet do on dinh"); Serial.println();        
    }
    else if(NhietDo >= GH_NhietDo_High) {
      digitalWrite(mayphunsuongPin, 1); lcd.setCursor(0, 3); lcd.print("Suong: ON");
      digitalWrite(maysuoiPin, 0); lcd.setCursor(10, 3); lcd.print("Suoi:OFF");
      digitalWrite(mayquatPin, 0);lcd.setCursor(10, 2); lcd.print("Quat:OFF");
      Serial.print("Nhiet do cao"); Serial.println();     
    }
    else {
      digitalWrite(mayphunsuongPin, 0); lcd.setCursor(0, 3); lcd.print("Suong:OFF");
      digitalWrite(maysuoiPin, 1); lcd.setCursor(10, 3); lcd.print("Suoi: ON");
      digitalWrite(mayquatPin, 0);lcd.setCursor(10, 2); lcd.print("Quat:OFF"); 
      Serial.print("Nhiet do thap"); Serial.println();           
    }
  }
  else if (DoAm >= GH_DoAm_High) {
    digitalWrite(mayphunsuongPin, 0); lcd.setCursor(0, 3); lcd.print("Suong:OFF");
    digitalWrite(maysuoiPin, 0); lcd.setCursor(10, 3); lcd.print("Suoi:OFF");
    digitalWrite(mayquatPin, 1);lcd.setCursor(10, 2); lcd.print("Quat: ON");
    Serial.print("Do Am cao"); Serial.println();    
  }
  else {
    digitalWrite(mayphunsuongPin, 1); lcd.setCursor(0, 3); lcd.print("Suong: ON");
    digitalWrite(maysuoiPin, 0); lcd.setCursor(10, 3); lcd.print("Suoi:OFF");
    digitalWrite(mayquatPin, 0); lcd.setCursor(10, 2); lcd.print("Quat:OFF");  
    Serial.print("Do am thap"); Serial.println();       
  }
}

void sendData(){
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
  myLoRa.write("\n");
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
  lcd.setCursor(3, 0);
  lcd.print(NhietDo);
  lcd.printf(" oC");
  lcd.setCursor(13, 0);
  lcd.print(DoAm);
  lcd.printf(" %");
}

void read_BH1750(){
  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx\n");
  //Firebase.setInt(dulieufirebase, "CuongDoAnhSang", lux);

  lcd.setCursor(3, 1);
  lcd.print(lux);
  lcd.printf(" lx");
}
