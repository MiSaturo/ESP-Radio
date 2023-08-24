# ESP Radio

A simple internet radio player based on ESP32 or ESP8266. 


# Compile
 
You need Arduino IDE to build. 

- install (ESP8266Audio)[https://github.com/earlephilhower/ESP8266Audio] 
- set clock to 160MHZ

# Circuit

For esp8266 without external DAC use GPIO3(RX):

![No Dec Circuit](I2SNoDac%20for%20ESP8266/no-dec-circuit.jpg)

# How to setup ffmpeg re-encoder

- copy `ffmpeg-radio` and it's contents to `/root`
- copy `systemd/ffmpeg-radio.service` to `/etc/systemd/system`
- auto start service by `systemctl enable ffmpeg-radio`
- run service by `systemctl start ffmpeg-radio`
