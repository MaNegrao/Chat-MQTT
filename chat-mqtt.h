#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1888" 
#define QOS         1
#define TIMEOUT     10000L

char PAYLOAD[256];
char USER_ID[4];
char USER_TOPIC[12];

FILE *online_users_file;

int mosquitto_lib_init();	