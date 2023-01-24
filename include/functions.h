#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "readsensors.h"

void ejection();

void ejectionTimerCallback(TimerHandle_t ejectionTimerHandle);

struct Data formart_data(SensorReadings readings, FilteredValues filtered_values);

float get_base_altitude();

#endif