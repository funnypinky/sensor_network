#include "mesh.h"

const long frequency = 868E6; // LoRa Frequency

void LoRa_rxMode()
{
    LoRa.enableInvertIQ(); // active invert I and Q signals
    LoRa.receive();        // set receive mode
}

void LoRa_txMode()
{
    LoRa.idle();            // set standby mode
    LoRa.disableInvertIQ(); // normal mode
}

void LoRa_sendMessage(String message)
{
    LoRa_txMode();        // set tx mode
    LoRa.beginPacket();   // start packet
    LoRa.print(message);  // add payload
    LoRa.endPacket(true); // finish packet and send it
}

void onReceive(int packetSize)
{
    String message = "";

    while (LoRa.available())
    {
        message += (char)LoRa.read();
    }

    Serial.print("Gateway Receive: ");
    Serial.println(message);
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

void onTxDone()
{
    Serial.println("TxDone");
    LoRa_rxMode();
}

void initMesh()
{

    randomSeed(analogRead(0));
    Serial.begin(9600);
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DI0);

    if (!LoRa.begin(frequency))
    {
        Serial.println("LoRa init failed. Check your connections.");
        while (true)
            ; // if failed, do nothing
    }

    Serial.println("LoRa init succeeded.");
    Serial.println();
    Serial.println("LoRa Simple Node");
    Serial.println("Only receive messages from gateways");
    Serial.println("Tx: invertIQ disable");
    Serial.println("Rx: invertIQ enable");
    Serial.println();

    LoRa.onReceive(onReceive);
    LoRa.onTxDone(onTxDone);
    LoRa_rxMode();
}

void sendData(String sendData)
{
    LoRa_sendMessage(sendData);
}

void timeSync()

{
    DynamicJsonDocument doc(1024);
    doc["cmd"] = "requestTimeSync";
    doc["source"] = WiFi.macAddress();
    String buffer;
    serializeJson(doc, buffer);
    sendData(buffer);
}