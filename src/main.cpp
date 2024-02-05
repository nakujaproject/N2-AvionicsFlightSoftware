#include <Arduino.h>
#include "checkstate.h"
#include "readsensors.h"
#include "transmitwifi.h"
#include "defs.h"
#include "kalmanfilter.h"
#include "functions.h"
#include "SPIFFS.h"

TimerHandle_t ejectionTimerHandle = NULL;

portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

TaskHandle_t WiFiTelemetryTaskHandle;

TaskHandle_t GetDataTaskHandle;

TaskHandle_t SPIFFSWriteTaskHandle;

TaskHandle_t GPSTaskHandle;

// if 1 chute has been deployed
uint8_t isChuteDeployed = 0;

/* Onboard logging */
// File file;

// determines whether we create an access point or connect to one
int access_point = 0;

float BASE_ALTITUDE = 0;

float previousAltitude;

volatile int state = 0;

static uint16_t wifi_queue_length = 100;
static uint16_t spiff_queue_length = 500;
static uint16_t gps_queue_length = 100;

static QueueHandle_t wifi_telemetry_queue;
static QueueHandle_t spiff_queue;
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
    digitalWrite(buzzer_pin, HIGH); 
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

  filtered_values = kalmanUpdate(readings.altitude, -readings.ax - 9.8);

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
    if (xQueueSend(spiff_queue, (void *)&ld, 0) != pdTRUE)
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

  
    vTaskDelay(10 / portTICK_PERIOD_MS);
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

  // setup flash memory
  if (!SPIFFS.begin(true))
    debugln("[-] An error occurred while mounting SPIFFS");
  else
    debugln("[+] SPIFFS mounted successfully");

  // get the base_altitude
  BASE_ALTITUDE = get_base_altitude();

  wifi_telemetry_queue = xQueueCreate(wifi_queue_length, sizeof(Data));
  spiff_queue = xQueueCreate(spiff_queue_length, sizeof(Data));
  gps_queue = xQueueCreate(gps_queue_length, sizeof(GPSReadings));

  // initialize core tasks
  xTaskCreatePinnedToCore(GetDataTask, "GetDataTask", 3000, NULL, 1, &GetDataTaskHandle, 0);
  xTaskCreatePinnedToCore(WiFiTelemetryTask, "WiFiTelemetryTask", 4000, NULL, 1, &WiFiTelemetryTaskHandle, 1);
  // xTaskCreatePinnedToCore(readGPSTask, "ReadGPSTask", 3000, NULL, 1, &GPSTaskHandle, 1);

  vTaskDelete(NULL);
}
void loop()
{
}