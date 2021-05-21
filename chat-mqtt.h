#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <unistd.h>
#include "MQTTAsync.h"

#define ADDRESS     "tcp://localhost:1883" 
#define QOS         1
#define TIMEOUT     10000L

#define GP1_TOPIC_ID "Secret_Jedis"
#define GP2_TOPIC_ID "Formula_1_Fans"
#define GP3_TOPIC_ID "Overwatch_Players"
#define GP4_TOPIC_ID "iRacing_Subscribers"
#define GP5_TOPIC_ID "Coding_Lovers"

char USER_ID[5];
char USER_ID_ID[3];
char USER_TOPIC_CONTROL[20]="";
char USER_TOPIC_CLIENT[20]="";

char TOPICS_ONLINE[99][20];
char GP_TOPICS_ONLINE[99][20];

int group_control = 1;
int disc_finished = 0, finished = 0;

FILE *users_topics;
FILE *subs_groups_file;

pthread_mutex_t pub_msg_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sub_topic_mutex = PTHREAD_MUTEX_INITIALIZER;
