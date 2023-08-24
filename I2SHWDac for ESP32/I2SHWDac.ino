#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.

// Enter your WiFi setup here:
const char *SSID = "WIFI_AP_NAME";
const char *PASSWORD = "WIFI_AP_PASSWORD";

//mp3 stream url
const char *URL = "http://ICE_CAST_SERVER:8000/payam";

AudioGeneratorMP3 *audio;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2) - 1] = 0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to WiFi");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }
  Serial.println("Connected");
  audioLogger = &Serial;
  initA();
}

void initA(){
  delete file;
  file = new AudioFileSourceICYStream(URL);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");

  delete buff;
  buff = new AudioFileSourceBuffer(file, 8192);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");

  delete out;
  out = new AudioOutputI2S(0,1);
  out->SetOutputModeMono(true);
  //out->SetGain(1);
  //out->SetRate(22050);

  delete audio;
  file->SetReconnect(3, 500);
  audio = new AudioGeneratorMP3();
  audio->RegisterStatusCB(StatusCallback, (void*)"audio");
  audio->begin(buff, out); 
}

void loop()
{
  static int lastms = 0;
  if (audio->isRunning()) {
    if (millis() - lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
    }
    if (!audio->loop()) audio->stop();
  } else {
    initA();
    Serial.printf("MP3 done\n");
    delay(1000);
  }
}
