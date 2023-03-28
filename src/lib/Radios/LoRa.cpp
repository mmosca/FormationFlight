
#include "LoRa.h"
#include "RadioManager.h"
#include "../CryptoManager.h"

void IRAM_ATTR onPacketReceive(void)
{
    LoRa::getSingleton()->flagPacketReceived();
}

LoRa *loraInstance = nullptr;

LoRa::LoRa()
{

}

LoRa* LoRa::getSingleton()
{
    if (loraInstance == nullptr)
    {
        loraInstance = new LoRa();
    }
    return loraInstance;
}

void LoRa::transmit(air_type0_t *air_0)
{
    if (!getEnabled()) {
        return;
    }
    uint8_t buf[sizeof(air_type0_t)];
    memcpy_P(buf, air_0, sizeof(air_type0_t));
    CryptoManager::getSingleton()->encrypt(buf, sizeof(air_type0_t));
    radio.transmit(buf, sizeof(air_type0_t));
    radio.startReceive();
}

int LoRa::begin() {
#ifdef HAS_LORA
    radio = new Module(LORA_PIN_CS, LORA_PIN_DIO, LORA_PIN_RST);
    radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, LORA_POWER, PREAMBLE_LENGTH);
    radio.setCRC(0);
    #ifdef LORA_PIN_RXEN
    radio.setRfSwitchPins(LORA_PIN_RXEN, LORA_PIN_TXEN);
    #endif
    radio.implicitHeader(sizeof(air_type0_t));
    radio.setDio1Action(onPacketReceive);
    radio.startReceive();
#endif
    return 0;
}

void LoRa::flagPacketReceived()
{
    if (!getEnabled()) {
        return;
    }
    packetReceived = true;
}

void LoRa::receive()
{
    uint8_t buf[sizeof(air_type0_t)];
    radio.readData(buf, sizeof(air_type0_t));
    CryptoManager::getSingleton()->decrypt(buf, sizeof(air_type0_t));
    handleReceiveCounters(RadioManager::getSingleton()->receive(buf, sizeof(air_type0_t), radio.getRSSI()));
}

void LoRa::loop()
{
    if (packetReceived)
    {
        receive();
        packetReceived = false;
    }
}

String LoRa::getStatusString()
{
    char buf[128];
#ifdef HAS_LORA
    sprintf(buf, "LoRa @ %fMHz (%ddBm) [%luTX/%luRX] [%luCRC/%luSIZE/%luVAL]", FREQUENCY, LORA_POWER, packetsTransmitted, packetsReceived, packetsBadCrc, packetsBadSize, packetsBadValidation);
#endif
    return String(buf);

}