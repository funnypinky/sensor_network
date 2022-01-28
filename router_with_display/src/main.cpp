#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include "hmi.hpp"
#include "lvgl.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{

    Serial.printf("Directory-Listing: %s\r\n", dirname);

    File root = fs.open(dirname);

    if (!root)
    { // Verzeichnis nicht vorhanden

        Serial.println("- Fehler beim Öffnen des Verzeichnisses");

        return;
    }

    if (!root.isDirectory())
    { // kein Verzeichnis, sondern Datei

        Serial.println(" - kein Verzeichnis");

        return;
    }

    File file = root.openNextFile();

    // in einer Schleife durch die Liste aller vorhandenen

    // Einträge gehen

    while (file)
    {

        if (file.isDirectory())
        {

            Serial.print("  DIR : ");

            Serial.println(file.name());

            // ist der Eintrag ein Verz., dann dessen Inhalt rekursiv

            // anzeigen, wenn maximale Tiefe noch nicht erreicht ist

            if (levels)
            {

                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {

            Serial.print("  FILE: ");

            Serial.print(file.name());

            Serial.print("\tGröße: ");

            Serial.println(file.size());
        }

        file = root.openNextFile();
    }
}

void setup()
{
    Serial.begin(9600);
    setupHMI();
}

void loop()
{
    lv_task_handler();
    delay(10);
}
