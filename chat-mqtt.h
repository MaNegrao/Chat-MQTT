#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883" 
#define QOS         2
#define TIMEOUT     10000L

char PAYLOAD[256];
char USER_ID[4];
char USER_TOPIC_CONTROL[12];
char USER_TOPIC_CLIENT[12];
char TO_ID[4];

FILE *online_users_file;