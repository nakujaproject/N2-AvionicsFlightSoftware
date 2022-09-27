#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x, y) Serial.printf(x, y)
#else
#define debug(x)
#define debugln(x)
#define debugf(x, y)
#endif

#define SEA_LEVEL_PRESSURE                  102400

// Timing delays
#define BAUD_RATE                           115200
#define GPS_BAUD_RATE                       9600

// Pin to start ejection charge
#define EJECTION_PIN                        4

#define PRE_FLIGHT_GROUND_STATE             0
#define POWERED_FLIGHT_STATE                1
#define COASTING_STATE                      2
#define BALLISTIC_DESCENT_STATE             3
#define CHUTE_DESCENT_STATE                 4
#define POST_FLIGHT_GROUND_STATE            5

#define GROUND_STATE_DISPLACEMENT           20
#define BELOW_APOGEE_LEVEL_DISPLACEMENT     20

#define GPS_TX_PIN                          17
#define GPS_RX_PIN                          16

extern const BaseType_t pro_cpu;
extern const BaseType_t app_cpu;

#define ssid                                "S7 edge"
#define password                            "almg76061"

// MQTT Broker IP address
#define mqtt_server                         "192.168.43.133"
#define MQQT_PORT                           1883

extern WiFiClient espClient;
extern PubSubClient client;

extern float BASE_ALTITUDE;
extern float MAX_ALTITUDE;

// This struct is used to save all our datapoints.
// It includes rocket altitude, accelerations in the x, y and z directions
// Gryroscope values in the x, y and z direcion
// filtered altitude, velocity and acceleration
// GPS longitude, laltitude and altitude and state
struct LogData
{
    uint64_t timeStamp;
    float altitude;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float filtered_s;
    float filtered_v;
    float filtered_a;
    int state;
    float latitude;
    float longitude;
    float gpsAltitude;
};
// SensorReadings contains the measurement we are getting
// from the sensors bmp and mpu
struct SensorReadings
{
    float altitude;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
};
// GPSReadings contains the gps informations that is
// latitude, longitude, speed, number of satellites and altitude
struct GPSReadings
{
    float latitude;
    float longitude;
    float speed;
    int satellites;
    float altitude;
};

// FilteredValues contains filtered values from the kalman filter
struct FilteredValues
{
    float displacement;
    float velocity;
    float acceleration;
};
// SendValues contains the data points we will be sending over lora
struct SendValues
{
    uint64_t timeStamp;
    float altitude;
    uint16_t state;
    float latitude;
    float longitude;
};

#endif
