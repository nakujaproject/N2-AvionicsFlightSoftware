#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>
#include "readsensors.h"
#include <TinyGPS++.h>

TinyGPSPlus gps;
Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;

void init_gps()
{
    // GPSModule.begin(GPS_BAUD_RATE);
    Serial1.begin(9600, SERIAL_8N1,GPS_RX_PIN,GPS_TX_PIN);
}

void init_mpu()
{
    debugln("MPU6050 test!");
    if (!mpu.begin())
    {
        debugln("Could not find a valid MPU6050 sensor, check wiring!");
        while (1)
        {
           // produces a sound with a frequency of 3khz
             tone(buzzer_pin,3000);
        }
    }
    else
    {
        debugln("MPU6050 FOUND");
        tone(buzzer_pin,3000);
        delay(1000);
        noTone(buzzer_pin);
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void init_bmp()
{
    debugln("BMP180 INITIALIZATION");
    if (!bmp.begin())
    {
        debugln("Could not find a valid BMP085 sensor, check wiring!");
        while (1)
        {
         //produces a sound with a frequency of 2khz
              tone(buzzer_pin,2000);
        }
    }
    else
    {
        debugln("BMP180 FOUND");
        tone(buzzer_pin,2000);
        delay(1000);
        noTone(buzzer_pin);
    }
}

// function to initialize bmp, mpu, lora module and the sd card module
void init_sensors()
{
    init_gps();
   init_bmp();
    init_mpu();
}


//get gps readings from the sensor
struct GPSReadings get_gps_readings()
{
    int stringplace = 0;
    int pos = 0;
    struct GPSReadings gpsReadings;
    
    while(Serial1.available() > 0 ){
 gps.encode(Serial1.read());
 if(gps.location.isUpdated()){
    //latitude in degrees
    gpsReadings.latitude = gps.location.lat();
    gpsReadings.longitude = gps.location.lng();
 }
    }
    return gpsReadings;
}

// Get the sensor readings
struct SensorReadings get_readings()
{
    struct SensorReadings return_val;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return_val.altitude = bmp.readAltitude(SEA_LEVEL_PRESSURE);
    return_val.ax = a.acceleration.x;
    return_val.ay = a.acceleration.y;
    return_val.az = a.acceleration.z;
    return_val.temperature=bmp.readTemperature();

    return_val.gx = g.gyro.x;
    return_val.gy = g.gyro.y;
    return_val.gz = g.gyro.z;

    return return_val;
}