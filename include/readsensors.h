#ifndef READSENSORS_H
#define READSENSORS_H

#include "defs.h"

void init_gps();

void init_mpu();

void init_bmp();

void init_sensors();

String ConvertLat(String nmea[15]);

String ConvertLng(String nmea[15]);

struct GPSReadings get_gps_readings();

struct SensorReadings get_readings();

#endif