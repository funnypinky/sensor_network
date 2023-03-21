#include "api.hpp"

HTTPClient sender;
WiFiClientSecure wifiClient;

IPAddress server(74, 125, 115, 105); // Google

StaticJsonDocument<3072> docWeather;

void readApi(float *min, float *max)
{
    wifiClient.setInsecure();
    if (sender.begin(wifiClient, "https://10.72.72.102:8443/api/data/forecast/"))
    {
        int httpCode = sender.GET();
        if (httpCode > 0)
        {
            // Anfrage wurde gesendet und Server hat geantwortet
            // Info: Der HTTP-Code für 'OK' ist 200
            if (httpCode == 200)
            {
                // Hier wurden die Daten vom Server empfangen
                // String vom Webseiteninhalt speichern
                String payload = sender.getString();
                // Hier kann mit dem Wert weitergearbeitet werden
                // ist aber nicht unbedingt notwendig
                StaticJsonDocument<48> filter;
                filter["daily"][0]["temp"] = true;

                DeserializationError error = deserializeJson(docWeather, payload, DeserializationOption::Filter(filter));

                // Test if parsing succeeds.
                if (error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                    return;
                }
                JsonArray array = docWeather["daily"].as<JsonArray>();
                JsonObject daily = array[0].as<JsonObject>();
                JsonObject daily_item_temp = daily["temp"];
                float daily_item_temp_min = daily_item_temp["min"];
                *min = daily_item_temp_min;
                float daily_item_temp_max = daily_item_temp["max"];
                *max = daily_item_temp_max;
            }
            else
            {
                // Falls HTTP-Error
                Serial.print("HTTP-Error: " + String(httpCode));
            }
        }
        // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
        sender.end();
    }
    else
    {
        Serial.printf("HTTP-Verbindung konnte nicht hergestellt werden!");
    }
}