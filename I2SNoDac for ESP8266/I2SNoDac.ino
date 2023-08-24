#include <ESP8266WiFi.h>
#include "AudioFileSourceHTTPStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.

// Enter your WiFi setup here:
const char *SSID = "WIFI_AP_NAME";
const char *PASSWORD = "WIFI_AP_PASSWORD";

//mp3 stream url
const char *URL = "http://ICE_CAST_SERVER:8000/payam";


AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SNoDAC *out;


int startTms = 0;


void initA() {
  //startTms=millis();
  Serial.printf("Reconnecting... \n");
  delete file;
  file = new AudioFileSourceHTTPStream(URL);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");

  delete buff;
  buff = new AudioFileSourceBuffer(file, 2048);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");

  delete out;
  out = new AudioOutputI2SNoDAC();
  //out->SetOutputModeMono(true);
  //out->SetGain(3);
  //out->SetRate(22050);

  delete mp3;
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  file->SetReconnect(3, 500);
  mp3->begin(buff, out);
}


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
  if (code==257){//out of sync
    //delay(1000);
  }
  
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to WiFi");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
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
void loop()
{
  static int lastms = 0;
  
  if (mp3->isRunning()) {
    if (millis() - lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms... [Free Memory: %d]\n", lastms,ESP.getFreeHeap());
      Serial.flush();
    }
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
    
    if (millis() - startTms > 10000) {
      
      
    }
    initA();
    delay(1000);
    
  }
}
