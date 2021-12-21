# esp32-bluetooth-audio

An ESP32 blueooth audio receiver, inspired by [KitchenRadio](https://github.com/jeroenlukas/KitchenRadio) and using [ESP32-A2DP library](https://github.com/pschatzmann/ESP32-A2DP) ! 🙏


## Hardware

 - esp32
 - Adafruit VS1053 breakout
 - jack connector


## Connections

Follow [Adafruit VS1053 wiring tutorial](https://learn.adafruit.com/adafruit-vs1053-mp3-aac-ogg-midi-wav-play-and-record-codec-tutorial/simple-audio-player-wiring) and use [SPI3 on esp32](https://techoverflow.net/2021/07/26/what-is-the-spi-pinout-of-the-esp32-esp-wroom-32/)


- VS1053 -> ESP32
---
- VCC  -> 3.3v
- GND  -> GND
- CLK  -> IO 18
- MISO -> IO 19
- MOSI -> IO 23
- CS   -> IO  5
- RST  -> RST
- XDCS -> IO 17
- SDCS -> IO 21
- DREQ -> IO 26
- AGND -> Center 'ground' pin on jack connector
- LOUT -> Left pin on jack connector
- ROUT -> Right pin on jack connector


## Librairies

- Adafruit VS1053 Library
- [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP.git)

## Flash

Flash using « ESP32 DEV Module »


## Ressources

- (VS1053 Datasheet)[https://www.sparkfun.com/datasheets/Components/SMD/vs1053.pdf]
- (Inspiring issue to connect ESP32-A2DP to VS1053)[https://github.com/pschatzmann/ESP32-A2DP/issues/31]
