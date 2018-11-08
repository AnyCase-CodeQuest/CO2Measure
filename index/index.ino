#define MH_Z19_RX 5
#define MH_Z19_TX 4
#define DHT_PIN 13
#define DHT_VERSION DHT22
#define MAX_DATA_ERRORS 15 //max of errors, reset after them
#define INTERVAL 5000
#define BLYNK_TOKEN "073fa089fbed404dbb1fbfc209b3811e"

#include <SoftwareSerial.h>
#include <DHT.h> // https://github.com/adafruit/DHT-sensor-library
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include "wifiCreds.h"

long previousMillis = 0;
int errorCount = 0;
SoftwareSerial co2Serial(MH_Z19_RX, MH_Z19_TX); // define MH-Z19
DHT dht(DHT_PIN, DHT_VERSION);//define temperature and humidity sensor

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  Serial.begin(115200); // Init console
  Serial.println("Setup started");

  unsigned long previousMillis = millis();
  co2Serial.begin(9600); //Init sensor MH-Z19
  dht.begin();
  Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PWD);
}

void loop()
{
  Blynk.run();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < INTERVAL)
    return;
  previousMillis = currentMillis;
  Serial.println("loop started");

  if (errorCount > MAX_DATA_ERRORS)
  {
    Serial.println("Too many errors, resetting");
    delay(2000);
    resetFunc();
  }
  Serial.println("reading data:");
  int ppm = readCO2();
  bool dataError = false;
  Serial.println("  PPM = " + String(ppm));
  Blynk.virtualWrite(V5, ppm);

  if (ppm < 100 || ppm > 6000)
  {
    Serial.println("PPM not valid");
    dataError = true;
  }
  int mem = ESP.getFreeHeap();
  Serial.println("  Free RAM: " + String(mem));

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, h);

  Serial.print("  Humidity = ");
  Serial.print(h, 1);
  Serial.print(", Temp = ");
  Serial.println(t, 1);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    dataError = true;
  }

  if (dataError)
  {
    Serial.println("Skipping loop");
    errorCount++;
    return;
  }
  errorCount = 0;

  Serial.println("loop finished");
  Serial.println("");
}

/**
 * Read from CO2 sensor
 * if error was occurred then -1 will be returned
 * @return int PPM
 */
int readCO2()
{
    // command to ask for data
    byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    // for answer
    byte response[9];

    co2Serial.write(cmd, 9); //request PPM CO2

    // The serial stream can get out of sync. The response starts with 0xff, try to resync.
    while (co2Serial.available() > 0 && (unsigned char)co2Serial.peek() != 0xFF) {
        co2Serial.read();
    }

    memset(response, 0, 9);
    co2Serial.readBytes(response, 9);

    if (response[1] != 0x86)
    {
        Serial.println("Invalid response from co2 sensor!");
        return -1;
    }

    byte crc = 0;
    for (int i = 1; i < 8; i++) {
        crc += response[i];
    }
    crc = 255 - crc + 1;

    if (response[8] == crc) {
        int responseHigh = (int) response[2];
        int responseLow = (int) response[3];
        int ppm = (256 * responseHigh) + responseLow;
        return ppm;
    } else {
        Serial.println("CRC error!");
        return -1;
    }
}
