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
const char *clientName = "MQTT_Gateway";

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
}

void taskSendFifo()
{
    while (!fifo.isEmpty())
    {
        Serial.printf("FiFo size %d \n", fifo.size());
        message item = fifo.first();
        publish(item.topic, item.payload);
        fifo.shift();
        Serial.printf("FiFo size %d \n", fifo.size());
    }
}
void publish(String topic, String message)
{
    reconnect();
    Serial.print("topic:");
    Serial.println(topic);
    Serial.print("payload:");
    Serial.println(message);
    client.publish(topic.c_str(), message.c_str());
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