#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --\n");
    printf("1. Iniciar um chat com um usuario\n");
    printf("2. Enviar mensagem\n");
    printf("3. Entrar em um grupo \n");
    printf("4. Enviar mensagem em um grupo \n");
	printf("5. Logout \n");
}

int pub_msg(char * topic, char * payload){
    int rc;

    MQTTClient_message message = MQTTClient_message_initializer;

    message.payload = payload;
    message.payloadlen = (int)strlen(payload);
    message.qos = QOS;

    if((rc = MQTTClient_publishMessage(client_mqtt, topic, &message, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to publish message, return code %d\n", rc);
        MQTTClient_destroy(client_mqtt);
        exit(0);
    }

}

int sub_topic(char * topic){
    int rc;
    if((rc = MQTTClient_subscribe(client_mqtt, topic, QOS)) != MQTTCLIENT_SUCCESS){
        printf("Falha ao assinar o tópico, Erro: %d\n", rc);
        MQTTClient_destroy(&client_mqtt);
        exit(0);
    }               
}


void *handle_new_chat(char * rec_user){
    char topic_rec[30]="", topic_chat[30]="";
    int session_id = (rand() % 100000);
    
    strncat(topic_rec, rec_user, 2);
    strcat(topic_rec, "_Client");

    strncat(topic_chat, rec_user, 2);
    strncat(topic_chat, USER_ID, 2);
    strcat(topic_chat, "_Chat");

    //sprintf(topic_chat, "%d_%.2s_%.2s_Chat", session_id, rec_user, USER_ID);

    // printf("%s\n", topic_rec);
    // printf("%s\n", topic_chat);

    pub_msg(topic_rec, topic_chat);
    pub_msg(topic_chat, "iniciando");

    sub_topic(topic_chat);

    // users_topics = fopen("users_topics.txt", "a+");

    // fprintf(users_topics, "%s\n", topic_chat);

    // fclose(users_topics);
}

int msg_arrvd(void *context, char *topic_name, int topic_len, MQTTClient_message *message){
    int rc;

    if(!strcmp(topic_name, USER_TOPIC_CONTROL)){
        printf("%.2s Inicou um chat com você\n", (char *)message->payload);
        handle_new_chat((char *)message->payload);
    }
    else if(!strcmp(topic_name, USER_TOPIC_CLIENT)){
        printf("Olha o chat iniciando!\n");
        sub_topic((char *)message->payload);
    }
    else{
        printf("Olha a mensagem!\n");
        printf("     topic: %s\n", topic_name);
        printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    }

    // printf("     topic: %s\n", topic_name);
    // printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    //MQTTClient_freeMessage(&message);
    //MQTTClient_free(topic_name);
    return 1;
}

void conn_lost(void *context, char *cause){
    printf("\nConexão perdida. Erro: %s\n", cause);
}

void ini_chat(){

    char rec_id[4], rec_topic[12] = "";

    printf("Qual ID do usuario que você deseja mandar mensagem?\n");
    
    __fpurge(stdin);
    fgets(rec_id, sizeof(rec_id), stdin);

    strncat(rec_topic, rec_id, 2);
    strcat(rec_topic, "_Control");

    printf("Requisitando inicio de sessão...\n");
    pub_msg(rec_topic, USER_ID);
}

// void sub_group(MQTTClient g_client){
//     int sel, rc;
//     char group_in_file[2];

//     printf("Em qual dos grupos abaixo você deseja entrar?\n");
//     printf("1 - Grupo dos Jedi's Ocultos\n");
//     printf("2 - Grupo dos Fãs de Formula 1\n");
//     printf("3 - Grupo dos jogadores de Overwatch\n");
//     printf("4 - Grupo dos Assinantes de iRacing\n");
//     printf("5 - Grupo dos Jedi's Ocultos\n");
    
//     scanf("%d", &sel);

//     if((rc = MQTTClient_subscribe(g_client, sel, QOS)) != MQTTCLIENT_SUCCESS)){
//         printf("Falha ao assinar o tópico, Erro: %d\n", rc);
//         MQTTClient_destroy(&g_client);
//         exit(0);
//     }
//     else{
//         printf("Cadastrado com sucesso!\n");
//     }
// }

void *main(){

    char user_in_file[4];
    int sel, rc;    

    printf("Digite seu ID único:\n");
    
    __fpurge(stdin);
    fgets(USER_ID, sizeof(USER_ID), stdin);

    strncat(USER_TOPIC_CONTROL, USER_ID, 2);
    strcat(USER_TOPIC_CONTROL, "_Control");

    strncat(USER_TOPIC_CLIENT, USER_ID, 2);
    strcat(USER_TOPIC_CLIENT, "_Client");

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    // conn_opts.keepAliveInterval = 20;
    // conn_opts.cleansession = 1;

    if((rc = MQTTClient_create(&client_mqtt, ADDRESS, USER_TOPIC_CONTROL, 
    MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Falha na criação do tópico do cliente. Erro: %d\n", rc);
        exit(0);
    }
    
    if((rc = MQTTClient_setCallbacks(client_mqtt, NULL, conn_lost, msg_arrvd, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Erro ao setar os callbacks. Erro: %d\n", rc);
        MQTTClient_destroy(&client_mqtt);
        exit(0);
    }

    if((rc = MQTTClient_connect(client_mqtt, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Conexão com o broker falhou. Erro: %d\n", rc);
        MQTTClient_destroy(&client_mqtt);
        exit(0);
    }

    if((rc = MQTTClient_subscribe(client_mqtt, USER_TOPIC_CONTROL, QOS)) != MQTTCLIENT_SUCCESS){
    	printf("Falha ao assinar o tópico, Erro: %d\n", rc);
        MQTTClient_destroy(&client_mqtt);
        exit(0);
    }

    if((rc = MQTTClient_subscribe(client_mqtt, USER_TOPIC_CLIENT, QOS)) != MQTTCLIENT_SUCCESS){
    	printf("Falha ao assinar o tópico, Erro: %d\n", rc);
        MQTTClient_destroy(&client_mqtt);
        exit(0);
    }

    //system("clear");
    printf("Bem-vindo! Agora você está online!\n\n");

    do
    {
        //system("clear");
        menu();
        scanf("%d", &sel);

        switch (sel){
            case 1:
                ini_chat();
                break;
            
            case 2:
                //sub_group(client_mqtt);
                break;

            case 3:
                break;

            case 4:
                break;

            case 5:
                MQTTClient_destroy(&client_mqtt);
                break;

            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (sel != 5);  
}

