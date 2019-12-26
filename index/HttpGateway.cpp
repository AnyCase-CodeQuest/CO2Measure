//
// Created by Mikalai Sahnouski on 11/27/18.
//

#include "HttpGateway.h"
#include <stdio.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// SHA1 fingerprint from browser about certificate
const uint8_t fingerprint[20] = {0x9D, 0x91, 0x1E, 0x60, 0xAB, 0x38, 0x2B, 0xE1, 0x8A, 0x23, 0x67, 0xAF, 0x1B, 0x9C, 0x96, 0xAC, 0x2E, 0x36, 0xE1, 0x25};

/**
 * curl -X POST "http://climate.any-case.info:1000/api/OfficeClimateSensors"
 * -H  "accept: application/json"
 * -H  "Content-Type: application/json"
 * -d "{  \"deviceId\": \"fa238a69-03ab-40d1-a51c-eb384844d243\",  \"cO2\": 3,  \"temperature\": 0,  \"humidity\": 0}"
 */
class HttpGateway {
private:
    unsigned short co2 = -1;
    char h = -1;
    float c = -128;

    const char * getJsonString() {
        char szBuffer[255];
        snprintf(szBuffer, sizeof(szBuffer), JSON_PATTERN, DEVICE_ID, this->co2, this->c, this->h);
        return szBuffer;
    }

public:

    void setTemperature(signed char temp) {
        this->c = temp;
    }

    void setHumidity(signed char humidity) {
        this->h = humidity;
    }

    void setCo2(unsigned short ppm) {
        this->co2 = ppm;
    }

    void run(byte * errorCount) {
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        HTTPClient http;    //Declare object of class HTTPClient
        client->setFingerprint(fingerprint);
        http.begin(*client, HOST_GATEWAY);      //Specify request destination
        http.addHeader("accept", "application/json");  //Specify content-type header
        http.addHeader("Content-Type", "application/json");  //Specify content-type header

        int httpCode = http.POST(this->getJsonString());   //Send the request
        String payload = http.getString();                  //Get the response payload

        Serial.println(httpCode);   //Print HTTP return code
        Serial.println(payload);    //Print request response payload
        /*if (httpCode != 200) {
            errorCount++;
        }*/

        http.end();  //Close connection
    }

};
