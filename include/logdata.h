#ifndef LOGDATA_H
#define LOGDATA_H

#include "defs.h"

void initSDCard();

char *printSDMessage(LogData ld);

void appendToFile(LogData ld[5]);


#endif
