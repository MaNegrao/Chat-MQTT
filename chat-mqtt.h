#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <unistd.h>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883" 
#define QOS         2
#define TIMEOUT     10000L

#define GP1_TOPIC_ID "Secret_Jedis"
#define GP2_TOPIC_ID "Formula_1_Fans"
#define GP3_TOPIC_ID "Overwatch_Players"
#define GP4_TOPIC_ID "iRacing_Subscribers"
#define GP5_TOPIC_ID "Coding_Lovers"

char USER_ID[4];
char USER_TOPIC_CONTROL[12]="";
char USER_TOPIC_CLIENT[12]="";


int session_id = 1;

FILE *users_topic;
FILE *subs_groups_file;

pthread_mutex_t scan_mutex;
pthread_t req_conn_thread;
pthread_t handle_conn_thread;

MQTTClient client_mqtt;
