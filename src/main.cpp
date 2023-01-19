#include <Arduino.h>
#include "checkstate.h"
#include "logdata.h"
#include "readsensors.h"
#include "transmitwifi.h"
#include "defs.h"
#include "kalmanfilter.h"
#include "functions.h"

TimerHandle_t ejectionTimerHandle = NULL;

portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

TaskHandle_t WiFiTelemetryTaskHandle;

TaskHandle_t GetDataTaskHandle;

TaskHandle_t SDWriteTaskHandle;

TaskHandle_t GPSTaskHandle;

// if 1 chute has been deployed
uint8_t isChuteDeployed = 0;

// determines whether we create an access point or connect to one
int access_point = 0;

float BASE_ALTITUDE = 0;

float previousAltitude;

volatile int state = 0;

static uint16_t wifi_queue_length = 100;
static uint16_t sd_queue_length = 500;
static uint16_t gps_queue_length = 100;

static QueueHandle_t wifi_telemetry_queue;
static QueueHandle_t sdwrite_queue;
static QueueHandle_t gps_queue;

// callback for done ejection
void ejectionTimerCallback(TimerHandle_t ejectionTimerHandle)
{
  digitalWrite(EJECTION_PIN, LOW);
  isChuteDeployed = 1;
}

// Ejection fires the explosive charge using a relay or a mosfet
void ejection()
{
  if (isChuteDeployed == 0)
  {
    digitalWrite(EJECTION_PIN, HIGH);
    // TODO: is 3 seconds enough?
    ejectionTimerHandle = xTimerCreate("EjectionTimer", 3000 / portTICK_PERIOD_MS, pdFALSE, (void *)0, ejectionTimerCallback);
    xTimerStart(ejectionTimerHandle, portMAX_DELAY);
  }
}

struct Data readData()
{
  struct Data ld = {0};
  struct SensorReadings readings = {0};
  struct FilteredValues filtered_values = {0};

  readings = get_readings();

  // TODO: very important to know the orientation of the altimeter

  filtered_values = kalmanUpdate(readings.altitude, readings.ax - 9.8);

  // using mutex to modify state
  portENTER_CRITICAL(&mutex);
  state = checkState(filtered_values.displacement, previousAltitude, filtered_values.velocity, filtered_values.acceleration, state);
  portEXIT_CRITICAL(&mutex);
  previousAltitude = filtered_values.displacement;
  ld = formart_data(readings, filtered_values);
  ld.state = state;
  ld.timeStamp = millis();

  return ld;
}

void GetDataTask(void *parameter)
{

  struct Data ld = {0};

  static int droppedWiFiPackets = 0;
  static int droppedSDPackets = 0;

  for (;;)
  {

    ld = readData();

    if (xQueueSend(wifi_telemetry_queue, (void *)&ld, 0) != pdTRUE)
    {
      droppedWiFiPackets++;
    }
    if (xQueueSend(sdwrite_queue, (void *)&ld, 0) != pdTRUE)
    {
      droppedSDPackets++;
    }

    debugf("Dropped WiFi Packets : %d\n", droppedWiFiPackets);
    debugf("Dropped SD Packets : %d\n", droppedSDPackets);

    // yield to WiFi Telemetry task
    vTaskDelay(74 / portTICK_PERIOD_MS);
  }
}

void WiFiTelemetryTask(void *parameter)
{

  struct Data sv = {0};
  struct Data svRecords;
  struct GPSReadings gpsReadings = {0};
  float latitude = 0;
  float longitude = 0;

  for (;;)
  {

    xQueueReceive(wifi_telemetry_queue, (void *)&sv, 10);
    svRecords = sv;
    svRecords.latitude = latitude;
    svRecords.longitude = longitude;

    if (xQueueReceive(gps_queue, (void *)&gpsReadings, 10) == pdTRUE)
    {
      latitude = gpsReadings.latitude;
      longitude = gpsReadings.longitude;
    }

    handleWiFi(svRecords);

    // yield to Get Data task
    vTaskDelay(35 / portTICK_PERIOD_MS);
  }
}

void readGPSTask(void *parameter)
{

  struct GPSReadings gpsReadings = {0};

  static int droppedGPSPackets = 0;

  for (;;)
  {
    gpsReadings = get_gps_readings();

    if (xQueueSend(gps_queue, (void *)&gpsReadings, 0) != pdTRUE)
    {
      droppedGPSPackets++;
    }

    debugf("Dropped GPS Packets : %d\n", droppedGPSPackets);

    // yield SD Write task
    // TODO: increase this up from 60 to 1000 in steps of 60 to improve queue performance at the expense of GPS
    // GPS will send 1 reading in 2s when set to 1000
    vTaskDelay(960 / portTICK_PERIOD_MS);
  }
}

void SDWriteTask(void *parameter)
{

  struct Data ld = {0};
  struct Data ldRecords[5];
  struct GPSReadings gps = {0};
  float latitude = 0;
  float longitude = 0;

  for (;;)
  {

    for (int i = 0; i < 5; i++)
    {
      xQueueReceive(sdwrite_queue, (void *)&ld, 10);

      ldRecords[i] = ld;
      ldRecords[i].latitude = latitude;
      ldRecords[i].longitude = longitude;

      if (xQueueReceive(gps_queue, (void *)&gps, 10) == pdTRUE)
      {
        latitude = gps.latitude;
        longitude = gps.longitude;
      }
    }

    appendToFile(ldRecords);

    // yield to GPS Task
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{

  Serial.begin(BAUD_RATE);

  // set up ejection pin
  pinMode(EJECTION_PIN, OUTPUT);
  digitalWrite(EJECTION_PIN, LOW);

  // set up buzzer pin
  pinMode(buzzer_pin, OUTPUT);

  if (access_point == 1)
  {
    create_Accesspoint();
  }
  else
  {

    setup_wifi();
  }

  init_sensors();

  // initSDCard();

  // get the base_altitude
  BASE_ALTITUDE = get_base_altitude();

  wifi_telemetry_queue = xQueueCreate(wifi_queue_length, sizeof(Data));
  sdwrite_queue = xQueueCreate(sd_queue_length, sizeof(Data));
  gps_queue = xQueueCreate(gps_queue_length, sizeof(GPSReadings));

  // initialize core tasks
  xTaskCreatePinnedToCore(GetDataTask, "GetDataTask", 3000, NULL, 1, &GetDataTaskHandle, 0);
  xTaskCreatePinnedToCore(WiFiTelemetryTask, "WiFiTelemetryTask", 4000, NULL, 1, &WiFiTelemetryTaskHandle, 0);
  //xTaskCreatePinnedToCore(readGPSTask, "ReadGPSTask", 3000, NULL, 1, &GPSTaskHandle, 1);
  // xTaskCreatePinnedToCore(SDWriteTask, "SDWriteTask", 4000, NULL, 1, &SDWriteTaskHandle, 1);

  vTaskDelete(NULL);
}
void loop()
{
}