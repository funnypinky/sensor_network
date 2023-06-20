#include "mesh.hpp"

bool Mesh::initMesh()
{
    LoRa.setPins(CS, RST, IRQ); // set CS, reset, IRQ pin
    if (!LoRa.begin(BAND))
    { // initialize ratio at 915 MHz
        Serial.println("LoRa init failed. Check your connections.");
        return false;
    }
    Serial.println("LoRa init succeeded.");
    return true;
}

void Mesh::sendMessage(String outgoing)
{
    LoRa.beginPacket();   // start packet
    LoRa.print(outgoing); // add payload
    LoRa.endPacket();     // finish packet and send it
}

String Mesh::onReceive(int packetSize)
{
    if (packetSize == 0 || packetSize > 200)
        return ""; // if there's no packet, return

    String incoming = "";

    while (LoRa.available())
    {
        incoming += (char)LoRa.read();
    }
    Serial.println("Message length: " + String(incoming.length()));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
    return incoming;
}

void Mesh::sleep() {
    LoRa.sleep();
}