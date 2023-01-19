#ifndef LOGDATA_H
#define LOGDATA_H

#include "defs.h"

void initSDCard();

char *printSDMessage(Data ld);

void appendToFile(Data ld[5]);


#endif