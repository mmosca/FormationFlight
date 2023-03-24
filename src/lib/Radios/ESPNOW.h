#include "RadioManager.h"
#ifdef PLATFORM_ESP8266
#include <espnow.h>
#include <ESP8266WiFi.h>
#elif defined(PLATFORM_ESP32)
#include <esp_now.h>
#include <WiFi.h>
#endif

class ESPNOW : public Radio
{
public:
    int begin();
    void transmit(air_type0_t *air_0);
    void loop();
private:
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#if defined(PLATFORM_ESP32)
    esp_now_peer_info_t peerInfo;
#endif
};