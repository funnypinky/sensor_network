#include "mqtt.hpp"

ConfigMQTT config;

WiFiClient espClient;
PubSubClient client = PubSubClient(espClient);
struct message
{
    String topic;
    String payload;
};
CircularBuffer<message, 25> fifo;
const char *clientName = "MQTT_Geiger";

bool connect()
{
    client.setServer(config.mqtt_ip, config.port);
    return client.connect(clientName, config.user.c_str(), config.pw.c_str());
}

void push2Fifo(String topic, String payload)
{
    message item;
    item.topic = topic;
    item.payload = payload;
    fifo.push(item);
    topic.clear();
    payload.clear();
}

void taskSendFifo()
{

    while (!fifo.isEmpty())
    {
        message item = fifo.first();
        Serial.print("topic:");
        Serial.println(item.topic);
        Serial.print("payload:");
        Serial.println(item.payload);
        publish(item.topic.c_str(), item.payload.c_str());
        fifo.shift();
    }
}
void publish(const char *topic, const char *payload)
{
    reconnect();
    client.publish(topic, payload);
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (connect())
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