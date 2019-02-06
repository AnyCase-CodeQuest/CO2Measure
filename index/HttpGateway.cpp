//
// Created by Mikalai Sahnouski on 11/27/18.
//

#include "HttpGateway.h"
#include <stdio.h>
#include <ESP8266HTTPClient.h>

/**
 * curl -X POST "http://climate.any-case.info:1000/api/OfficeClimateSensors"
 * -H  "accept: application/json"
 * -H  "Content-Type: application/json"
 * -d "{  \"deviceId\": \"fa238a69-03ab-40d1-a51c-eb384844d243\",  \"cO2\": 3,  \"temperature\": 0,  \"humidity\": 0}"
 */
class HttpGateway {
private:
    unsigned short co2 = -1;
    float h = -1;
    float c = -128.0;

    const char * getJsonString() {
        char szBuffer[255];
        snprintf(szBuffer, sizeof(szBuffer), JSON_PATTERN, DEVICE_ID, this->co2, this->c, this->h);
        Serial.print("debug JSON: ");
        Serial.println(szBuffer);
        return szBuffer;
    }

public:

    void setTemperature(float temp) {
        this->c = temp;
    }

    void setHumidity(float humidity) {
        this->h = humidity;
    }

    void setCo2(unsigned short ppm) {
        this->co2 = ppm;
    }

    void run(byte * errorCount) {
        HTTPClient http;    //Declare object of class HTTPClient

        http.begin(HOST_GATEWAY);      //Specify request destination
        http.addHeader("accept", "application/json");  //Specify content-type header
        http.addHeader("Content-Type", "application/json");  //Specify content-type header

        int httpCode = http.POST(this->getJsonString());   //Send the request
        String payload = http.getString();                  //Get the response payload

        Serial.print("HTTP return code: ");
        Serial.println(httpCode);   //Print HTTP return code
        Serial.print("request response payload: ");
        Serial.println(payload);    //Print request response payload
        if (httpCode != 200) {
            errorCount++;
        }

        http.end();  //Close connection
    }

};
