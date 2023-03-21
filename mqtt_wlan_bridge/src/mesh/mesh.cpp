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

void Mesh::onReceive(int packetSize)
{
    int hours, minutes, seconds, day, month, year;
     time_t now;
 
    // Aktuelle Zeit erhalten
    // `time()` gibt die aktuelle Uhrzeit des Systems als `time_t`-Wert zurück
    time(&now);
    struct tm *local = localtime(&now);
 
    hours = local->tm_hour;         // Stunden seit Mitternacht abrufen (0-23)
    minutes = local->tm_min;        // Minuten nach der Stunde vergehen lassen (0-59)
    seconds = local->tm_sec;        // Sekunden nach einer Minute verstreichen lassen (0-59)
 
    day = local->tm_mday;            // Tag des Monats abrufen (1 bis 31)
    month = local->tm_mon + 1;      // Monat des Jahres abrufen (0 bis 11)
    year = local->tm_year + 1900;

    if (packetSize == 0 || packetSize > 200)
        return; // if there's no packet, return

    String incoming = "";

    while (LoRa.available())
    {
        incoming = LoRa.readString();
    }
    Serial.println("Message length: " + String(packetSize));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    String info = "RSSI: "+String(LoRa.packetRssi()); 
    char timeInfo[25];
    snprintf(timeInfo, sizeof(timeInfo),"%02d/%02d/%d %02d:%02d", day, month, year,hours,minutes);
    logInfo logInfo;
    logInfo.timeStamp = String(timeInfo);
    logInfo.rssi = info;
    writeLog(logInfo);
    Serial.println();
    parseString(incoming);
}

void Mesh::parseString(String toParse)
{
    DynamicJsonDocument doc(512);
    std::vector<String> measVec = splitString(toParse,';');
    for(String item : measVec) {
        Serial.println(item);
    }
        if (measVec[0] == "data")
        {
            String topic = measVec[1];
            doc["time"] = measVec[7].toInt();
            doc["src"] = measVec[1];
            doc["values"]["battery"] = measVec[2].toDouble();
            doc["values"]["panel"] = measVec[3].toDouble();
            doc["values"]["temperature"] = measVec[4].toDouble();
            doc["values"]["humidity"] = measVec[5].toDouble();
            doc["values"]["pressure"] = measVec[6].toDouble();
            String buf;
            serializeJson(doc, buf);
            Serial.println(buf);
            publish(topic.c_str(), buf.c_str());
            buf.clear();
            topic.clear();
            doc.clear();
            Serial.println(ESP.getFreeHeap());
        }
        if(measVec[0] == "reqTime") {
            String tx;
            tx += "repTime;";
            tx += measVec[1]+";";
            tx += String(getTime()) + ";";
            sendMessage(tx);
        }
    }

void Mesh::sendTimeSync()
{
    StaticJsonDocument<1024> doc;
    doc["cmd"] = "responseTimeSync";
    doc["syncTime"] = getTime();
    String buffer = "";
    serializeJson(doc, buffer);
    sendMessage(buffer);
    buffer.clear();
}
unsigned long Mesh::getTime()
{
    time_t now;
    time(&now);
    return now;
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