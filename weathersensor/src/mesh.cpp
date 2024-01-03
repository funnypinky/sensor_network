#include "mesh.hpp"

bool receivedFlag = false;

bool Mesh::initMesh()
{
    int state = loraModule.begin();
    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.println("LoRa init failed. Check your connections.");
        return false;
    }
    loraModule.setFrequency(FREQUENCY);
    Serial.println("LoRa init succeeded.");
    return true;
}

void Mesh::sendMessage(String outgoing)
{       
    int state = loraModule.transmit(outgoing);
    if(state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        Serial.println(F("too long"));
    }
}

String Mesh::onReceive(int packetSize)
{
    if (packetSize == 0 || packetSize > 200)
        return ""; // if there's no packet, return

    String incoming = "";

    while (loraModule.available())
    {
        int state = loraModule.readData(incoming);
    }
    Serial.println("Message length: " + String(incoming.length()));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(loraModule.getRSSI()));
    Serial.println("Snr: " + String(loraModule.getSNR()));
    Serial.println();
    loraModule.standby();
    return incoming;
}

void Mesh::sleep() {
    loraModule.sleep();
}

void setFlag(void) {
    receivedFlag = true;
}

void Mesh::syncTime() {
  String reqTime;
  reqTime += "reqTime;";
  reqTime += WiFi.macAddress()+";";
  sendMessage(reqTime);
  loraModule.setPacketReceivedAction(setFlag);
  loraModule.startReceive();

  while(!receivedFlag) {
    receivedFlag = false;
    String answer;
    int state = loraModule.readData(answer);
    if(state == RADIOLIB_ERR_NONE) {
        if(answer != "") {
            std::vector<String> measVec = splitString(answer,';');
            if(measVec[0] == "repTime") {
            rtc.setTime(atol(measVec[2].c_str()));
            Serial.println(rtc.getTime());
        }
        }
    }
  }
}

std::vector<String> Mesh::splitString(String string, char delim) 
{
    String temp = string.substring(0, string.length());
    std::vector<String> returnVector;
    int index = string.indexOf(delim);
    while (index != -1) {
        returnVector.push_back(temp.substring(0,index));
        temp = temp.substring(index+1,string.length());
        index = temp.indexOf(delim);
    }
    return returnVector;
}
