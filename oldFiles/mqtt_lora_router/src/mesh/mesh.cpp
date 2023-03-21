#include "mesh.hpp"
#include <string>

const long frequency = 868E6;
RH_RF95 driver(18, 26);
RHReliableDatagram manager(driver, SERVER_ADDRESS);
U8G2LOG *u8g2logMesh;
int recvPacket;

const int
str2Int(String value)
{
    return value.toInt();
}

bool Mesh::initMesh(U8G2LOG *u8g2log)
{
    SPI.begin(5, 19, 27, 18);
    u8g2logMesh = u8g2log;
    recvPacket = 0;
    // LoRa.setPins(18, 14, 26);
    driver.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
    if (!manager.init())
    {
        u8g2log->println("LoRa init failed. Check your connections.");
        while (true)
            ; // if failed, do nothing
    }

    u8g2logMesh->println("LoRa init succeeded.");
    u8g2logMesh->println();
    u8g2logMesh->println("LoRa Simple Gateway");
    u8g2logMesh->println();
    return true;
}

void Mesh::taskRec()
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
        recvPacket++;
        u8g2logMesh->printf("Recv packet: %i",recvPacket);
        Serial.print("got reply: ");
        Serial.println((char *)buf);
        delay(100);
        StaticJsonDocument<1024> doc;
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
        if (type == "requestTimeSync")
        {
            sendTimeSync(from);
        }
        else
        {
            if (type == "routeData")
            {
                String topic = doc["source"];
                String buf;
                serializeJson(doc, buf);
                publish(topic.c_str(), buf.c_str());
                buf.clear();
                topic.clear();
                doc.clear();
                Serial.println(ESP.getFreeHeap());
            }
            else
            {
                Serial.println("Unknow data");
            }
        }
    }
}
void Mesh::LoRa_sendMessage(char *message, int len, uint8_t to)
{
    manager.sendtoWait((uint8_t *)message, len, to);
}
void Mesh::sendTimeSync(uint8_t to)
{
    StaticJsonDocument<1024> doc;
    doc["cmd"] = "responseTimeSync";
    doc["source"] = WiFi.macAddress();
    doc["syncTime"] = getTime();
    String buffer = "";
    serializeJson(doc, buffer);
    LoRa_sendMessage((char *)buffer.c_str(), buffer.length(), to);
    buffer.clear();
}
unsigned long Mesh::getTime()
{
    time_t now;
    time(&now);
    return now;
}