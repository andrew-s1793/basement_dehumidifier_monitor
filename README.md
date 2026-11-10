# basement_dehumidifer_monitor
This project automates monitoring of a basement dehumidifier unit that previously relied on visual and physical monitoring.
I used an ESP32 board, a DHT22 humidity sensor, and an LDR light sensor, all wired to a breadboard and placed on the unit to measure both the humidity levels
as well as the "tank full" LED that fires when the water tank of the unit fills up. The ESP32 board runs C++ code and was programmed in the Arduino IDE,
which allows it to connect to my home network and send notifications on the unit's status through the ntfy app, as well as log trends in an Adafruit IO dashboard. 
I programmed setpoint logic with hysteresis (alarm at 45% humidity, reset at 40%), calibrated the LDR threshold from measured on/off readings, and tested both alert paths end to end.
