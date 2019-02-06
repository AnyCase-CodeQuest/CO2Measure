//
// Created by Mikalai Sahnouski on 11/27/18.
//

#ifndef CO2MEASURE_HTTPGATEWAY_H
#define CO2MEASURE_HTTPGATEWAY_H
#define HOST_GATEWAY "http://climate.any-case.info:1000/api/OfficeClimateSensors"
#define DEVICE_ID "fa238a69-03ab-40d1-a51c-eb384844d243"
#define JSON_PATTERN "{\"deviceId\": \"%s\",  \"cO2\": %u,  \"temperature\": %f,  \"humidity\": %f}"

#endif //CO2MEASURE_HTTPGATEWAY_H
