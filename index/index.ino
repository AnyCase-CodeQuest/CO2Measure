#define MH_Z19_RX 5
#define MH_Z19_TX 4
#define MH_Z19_K 0.400000
#define DHT_PIN 13
#define DHT_VERSION DHT22
#define INTERVAL 5000
#define MAX_DATA_ERRORS 15

#include <SoftwareSerial.h>
#include <DHT.h> // https://github.com/adafruit/DHT-sensor-library
#include <ESP8266WiFi.h>
#include "wifiCreds.h"
#include <MHZ19.h>
#include "HttpGateway.cpp"

unsigned long _time = 0;

SoftwareSerial co2Serial(MH_Z19_RX, MH_Z19_TX); // define MH-Z19
MHZ19 mhz(&co2Serial);
DHT dht(DHT_PIN, DHT_VERSION);//define temperature and humidity sensor
HttpGateway gw;

char _command[100];
byte _idx = 0;
bool _readCommand = false;
byte errorCount = 0;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  Serial.begin(115200); // Init console
  Serial.println("Setup started");

  unsigned long previousMillis = millis();
  
  dht.begin();
  co2Serial.begin(9600);
  setupWifi(WIFI_SSID, WIFI_PWD);
}

void setupWifi(char * ssid, char * pwd)
{
    WiFi.begin(ssid, pwd);   //WiFi connection

    while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

        delay(500);
        Serial.println("Waiting for connection");

    }
}

void checkErrors()
{
  uint8_t wst = WiFi.status();
  Serial.println("WIFI status:");
  Serial.println(wst);
  if (errorCount > MAX_DATA_ERRORS || wst != WL_CONNECTED)
  {
    Serial.println("Too many errors, resetting");
    delay(2000);
    resetFunc();
  }
}

bool readDHT()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    errorCount++;
    return false;
  }
  gw.setTemperature(t);
  gw.setHumidity(h);
//  Blynk.virtualWrite(V6, t);
//  Blynk.virtualWrite(V7, h);
  return true;
}

bool readMHZ()
{
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
    Serial.print(F("CO2: "));
    int ppm = mhz.getCO2();
    Serial.println(ppm);
    Serial.println(ppm * MH_Z19_K);
    Serial.print(F("Temperature: "));
    Serial.println(mhz.getTemperature());
    Serial.print(F("Accuracy: "));
    Serial.println(mhz.getAccuracy());
    gw.setCo2(ppm);
//    Blynk.virtualWrite(V5, ppm);
  } else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
    errorCount++;
    return false;
  }
  Serial.println();

  return true;
}

void loop()
{
  unsigned long ms = millis();

  if (ms - _time > 15000 || _time > ms)
  {
    _time = ms;
    bool mhz = readMHZ();
    bool dht = readDHT();
    gw.run(&errorCount);
    checkErrors();
  }

  errorCount = 0;
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 13)
    {
      continue;
    }
    else if (c == 10)
    {
      _command[_idx] = '\0';
      _readCommand = true;
      _idx = 0;
    }
    else
    {
      _command[_idx] = c;
      _idx++;
    }
  }

  if (_readCommand)
  {
    _readCommand = false;

    if (strcmp(_command, "calibrate") == 0)
    {
      Serial.println(F("Calibration..."));
      mhz.calibrateZero();
    }
    if (strcmp(_command, "range2000") == 0)
    {
      Serial.println(F("Set range 2000..."));
      mhz.setRange(MHZ19_RANGE_2000);
    }
    if (strcmp(_command, "range5000") == 0)
    {
      Serial.println(F("Set range 5000..."));
      mhz.setRange(MHZ19_RANGE_5000);
    }
    if (strcmp(_command, "autoCOn") == 0)
    {
      Serial.println(F("Set auto calibration ON..."));
      mhz.setAutoCalibration(true);
    }
    if (strcmp(_command, "autoCOff") == 0)
    {
      Serial.println(F("Set auto calibration OFF..."));
      mhz.setAutoCalibration(false);
    }
  }
}
