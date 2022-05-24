#pragma once

#include "config_mqtt.hpp"
#include <WiFi.h>
#include <PubSubClient.h>
#include <CircularBuffer.h>

bool connect();
void reconnect();
void publish(const char *topic, const char *payload);
void push2Fifo(String topic, String payload);
void taskSendFifo();
