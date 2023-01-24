#include "defs.h"
#include "functions.h"

// formats data that we are going to save to SD card
// We save all the data points we are collecting
struct Data formart_data(SensorReadings readings, FilteredValues filtered_values)
{
    struct Data ld = {0};
    ld.altitude = readings.altitude;
    ld.temperature = readings.temperature;
    ld.ax = readings.ax;
    ld.ay = readings.ay;
    ld.az = readings.az;
    ld.gx = readings.gx;
    ld.gy = readings.gy;
    ld.gz = readings.gz;
    ld.filtered_s = filtered_values.displacement;
    ld.filtered_a = filtered_values.acceleration;
    ld.filtered_v = filtered_values.velocity;
    return ld;
}

// get_base_altitude Finds the average of the current altitude from 100 readings
float get_base_altitude()
{
    float altitude = 0;
    SensorReadings readings;
    for (int i = 0; i < 100; i++)
    {
        readings = get_readings();
        altitude = altitude + readings.altitude;
    }
    altitude = altitude / 100.0;
    debugf("Base Altitude is %.3f\n", altitude);
    return altitude;
}