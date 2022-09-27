#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "readsensors.h"

void ejection();

void ejectionTimerCallback(TimerHandle_t ejectionTimerHandle);

struct LogData formart_SD_data(SensorReadings readings, FilteredValues filtered_values);

struct SendValues formart_send_data(LogData readings);

float get_base_altitude();

#endif
