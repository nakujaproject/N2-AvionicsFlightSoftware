#ifndef LOGDATA_H
#define LOGDATA_H

#include "FS.h"
#include "SD.h"
#include <SPI.h>

File dataFile;

void initSDCard()
{
    if (!SD.begin())
    {
        debugln("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        debugln("No SD card attached");
        return;
    }
    debug("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        debugln("MMC");
    }
    else if (cardType == CARD_SD)
    {
        debugln("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        debugln("SDHC");
    }
    else
    {
        debugln("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    debugf("SD Card Size: %lluMB\n", cardSize);
}

char *printSDMessage(LogData ld)
{
    // The assigned size is calculated to fit the string
    char *message = (char *)pvPortMalloc(256);
    if (!message)
        return NULL;
    snprintf(message, 256, "{\"timestamp\":%lld,\"sensor altitude\":%.3f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,\"filtered s\":%.3f,\"filtered v\":%.3f,\"filtered a\":%.3f,\"state\":%d,\"gps altitude\":%.3f,\"longitude\":%.8f,\"latitude\":%.8f}\n", ld.timeStamp, ld.altitude, ld.ax, ld.ay, ld.az, ld.gx, ld.gy, ld.gz, ld.filtered_s, ld.filtered_v, ld.filtered_a, ld.state, ld.gpsAltitude, ld.longitude, ld.latitude);
    return message;
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendToFile(LogData ld[5])
{

    dataFile = SD.open("/telmetry.txt", FILE_APPEND);
    if (!dataFile)
    {
        debugln("Failed to open file for appending");
        return;
    }
    char combinedMessage[1280];
    strcpy(combinedMessage, "");
    for (int i = 0; i < 5; i++)
    {

        char *message = printSDMessage(ld[i]);
        strcat(combinedMessage, message);
        vPortFree(message);
    }

    if (dataFile.println(combinedMessage))
    {
        debugln("Message appended");
    }
    else
    {
        debugln("Append failed");
    }

    dataFile.close();
}


#endif
