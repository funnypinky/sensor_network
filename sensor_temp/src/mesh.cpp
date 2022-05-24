#include "mesh.h"

const long frequency = 868E6; // LoRa Frequency

RH_RF95 driver(SS, DI0);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void LoRa_sendMessage(char *message, int len)
{
    manager.sendtoWait((uint8_t *)message, len, SERVER_ADDRESS);
}

void taskReceive()
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;

    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {

        Serial.print("Gateway Receive: ");
        Serial.println((char *)buf);
        DynamicJsonDocument doc(1024);
        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, (char *)buf);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        String type = doc["cmd"];
        if (type == "responseTimeSync")
        {
            struct timeval tv;
            tv.tv_sec = doc["syncTime"]; // epoch time (seconds)
            tv.tv_usec = 0;              // microseconds
            settimeofday(&tv, NULL);
        }
        else
        {
            Serial.println("Unknow data");
        }
    }
    else
    {
        Serial.println("No reply, is rf95_reliable_datagram_server running?");
    }
}
void initMesh()
{
    randomSeed(analogRead(0));
    Serial.begin(9600);
    SPI.begin(SCK, MISO, MOSI, SS);

    if (!manager.init())
    {
        Serial.println("LoRa init failed. Check your connections.");
        while (true)
            ; // if failed, do nothing
    }

    Serial.println("LoRa init succeeded.");
    Serial.println();
    Serial.println("LoRa Simple Node");
    Serial.println("Only receive messages from gateways");
    Serial.println();
}

void sendData(char *sendData, int len)
{
    LoRa_sendMessage(sendData, len);
}

void timeSync()

{
    DynamicJsonDocument doc(1024);
    doc["cmd"] = "requestTimeSync";
    doc["source"] = WiFi.macAddress();
    String buffer;
    serializeJson(doc, buffer);
    sendData((char *)buffer.c_str(), buffer.length());
    taskReceive();
}