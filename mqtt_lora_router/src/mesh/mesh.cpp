#include "mesh.hpp"

const long frequency = 868E6;

const int str2Int(String value)
{
    return value.toInt();
}

bool Mesh::initMesh(U8G2LOG *u8g2log)
{
    SPI.begin(5, 19, 27, 18);
    LoRa.setPins(18, 14, 26);

    if (!LoRa.begin(frequency))
    {
        u8g2log->println("LoRa init failed. Check your connections.");
        while (true)
            ; // if failed, do nothing
    }
    u8g2log->println("LoRa init succeeded.");
    u8g2log->println();
    u8g2log->println("LoRa Simple Gateway");
    u8g2log->println();
    LoRa.onReceive(this->onReceive);
    LoRa.onTxDone(this->onTxDone);
    this->LoRa_rxMode();
    return true;
}
void Mesh::LoRa_rxMode()
{
    LoRa.disableInvertIQ(); // normal mode
    LoRa.receive();         // set receive mode
}

void Mesh::LoRa_txMode()
{
    LoRa.idle();           // set standby mode
    LoRa.enableInvertIQ(); // active invert I and Q signals
}
void Mesh::onTxDone()
{
    Serial.println("TxDone");
    LoRa_rxMode();
}
void Mesh::onReceive(int packetSize)
{
    String message = "";

    while (LoRa.available())
    {
        message += (char)LoRa.read();
    }
    delay(100);
    Serial.print("Gateway Receive: ");
    DynamicJsonDocument doc(1024);
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, message);

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
        Serial.println("Request timeSync");
        sendTimeSync();
    }
    else
    {
        if (type == "routeData")
        {
            String topic = doc["source"];
            Serial.println("Data routing to mqtt");
            push2Fifo(topic, message);
        }
        else
        {
            Serial.println("Unknow data");
        }
    }
}
void Mesh::LoRa_sendMessage(String message)
{
    LoRa_txMode();
    LoRa.beginPacket();   // start packet
    LoRa.print(message);  // add payload
    LoRa.endPacket(true); // finish packet and send it
}
void Mesh::sendTimeSync()
{
    DynamicJsonDocument doc(1024);
    doc["cmd"] = "responseTimeSync";
    doc["source"] = WiFi.macAddress();
    doc["syncTime"] = getTime();
    String buffer;
    serializeJson(doc, buffer);
    LoRa_sendMessage(buffer);
}
unsigned long Mesh::getTime()
{
    time_t now;
    time(&now);
    return now;
}