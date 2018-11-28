# CO2Measure
ESP8266 and MH-Z19B

# Quick start with ESP8266 (ESP-07)
* VCC = 3.3V
* Logic is for 3.3V
## Normal mode
* GPIO15 to GND
* CH_PD to VCC
## Flash mode
* GPIO0 to GND (IMPORTANT)
* CH_PD to VCC
* GPIO15 to GND
### How to flash with esptool.py
```# python PATH_TO/esptool.py -p /dev/SOME_USBSERIAL_DEVICE write_flash 0x000000 YOUR_FIRMWARE.bin```
## Connect ESP to MH-Z19B
Accuracy: ± (50ppm+3% reading value)

* GPIO5 to RX
* GPIO4 to TX
* VCC = 4.5 ~ 5.5 V DC
* GND

# DHT22
* VCC = 3.3V
* DAT = GPIO13

# Arduino IDE
We need to import ESP8266 board from that URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json

# REST backend
Each push
``
   curl -X POST "http://climate.any-case.info:1000/api/OfficeClimateSensors"
   -H  "accept: application/json"
   -H  "Content-Type: application/json"
   -d "{  \"deviceId\": \"fa238a69-03ab-40d1-a51c-eb384844d243\",  \"cO2\": 3,  \"temperature\": 0,  \"humidity\": 0}"
``
