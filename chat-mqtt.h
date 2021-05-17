#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio_ext.h>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883" 
#define QOS         1
#define TIMEOUT     10000L

char PAYLOAD[256];
char USER_ID[4];
char USER_TOPIC[10];

FILE *online_users_file;