#pragma once

#include <U8x8lib.h>
#include <string>
#include <CircularBuffer.h>

struct logInfo
{
    String timeStamp;
    String rssi;
};

void initDisplay();
void writeInfo(int row, const char *info, bool little = false);
void writeDateTime(const char *date, const char *time);
void writeLog(logInfo info);