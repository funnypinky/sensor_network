#pragma once

#include "config_mqtt.hpp"
#include <WiFi.h>
#include <PubSubClient.h>
#include <CircularBuffer.h>

bool connect();
void reconnect();
void publish(String topic, String message);
void push2Fifo(String topic, String payload);
void taskSendFifo();
