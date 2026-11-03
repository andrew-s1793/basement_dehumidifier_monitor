# basement_dehumidifer_monitor
This project aims to automate monitoring of a basement dehumidifer unit that currently relies on visual and physical monitoring.
I used an ESP-32 board, humidity sensor, and LED sensor all wired to a breadboard and placed on the unit to measure both the humidity levels,
as well as the "tank full" LED that fires when the water tank of the unit fills up. The ESP-32 board was programmed in Arduino and runs C++ code,
which allows it to connect to my home network and send notifications on the unit's status through the NTFY app, as well as log trends in an AdaFruit dashboard.