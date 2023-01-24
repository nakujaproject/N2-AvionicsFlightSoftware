#include "defs.h"

const BaseType_t pro_cpu = 0;
const BaseType_t app_cpu = 1;

WiFiClient espClient;
PubSubClient client(espClient);