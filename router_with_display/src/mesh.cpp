#include "mesh.h"

DynamicJsonDocument doc(200);

WiFiClient espClient;
PubSubClient client(espClient);

SPIClass LoRaSPI(HSPI);

const char *mqtt_server = "10.72.72.124";

void initMesh()
{
    SPI.begin(SCK, MISO, MOSI, SS);
    SPI.setFrequency(SPI_FREQUENCY);
    LoRa.setPins(SS, RST, DI0);

    while (!LoRa.begin(866E6))
    {
        Serial.println(".");
        delay(500);
    }
    // Change sync word (0xF3) to match the receiver
    // The sync word assures you don't get LoRa messages from other LoRa transceivers
    // ranges from 0-0xFF
    LoRa.setSyncWord(0xF3);
    // register the receive callback

    // put the radio into receive mode
    client.setServer(mqtt_server, 1883);
    void reconnect();
    // register the receive callback
    Serial.println("LoRa Initializing OK!");
}

void sendData(String data)
{
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
}

void recvData()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packet
        Serial.print("Received packet '");

        // read packet
        while (LoRa.available())
        {
            String LoRaData = LoRa.readString();
            Serial.print(LoRaData);
            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, LoRaData);

            // Test if parsing succeeds.
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }
            if (!client.connected())
            {
                reconnect();
            }
            float temperatur = doc["temperature"];
            float humidity = doc["humidity"];
            updateOutdoorValues(temperatur, humidity);
            String topic = "sensor/";
            String source = doc["source"];
            topic.concat(source);
            client.publish(topic.c_str(), LoRaData.c_str());
        }

        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }
}
void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("arduinoClient", "broker", "Rechner123"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}