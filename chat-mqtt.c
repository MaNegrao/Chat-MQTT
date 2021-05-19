#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --\n");
    printf("1. Iniciar um chat com um usuario\n");
    printf("2. Entrar em um grupo \n");
    printf("3. Enviar mensagem em um grupo \n");
	printf("4. Logout \n");
}

void logout(){
    char user[4];

    online_users_file = fopen("online-users.txt", "r+");

    flockfile(online_users_file);

    while(fgets(user, 4, online_users_file)){
        if(!strcmp(user, USER_ID)){
            fseek(online_users_file, -3, SEEK_CUR);
            fprintf(online_users_file, "XX");
        }
    }

    funlockfile(online_users_file);
    fclose(online_users_file);

}

int ver_usr(char * usr_cmp){
    char user_in_file[4];
    int flag = 1;

    online_users_file = fopen("online-users.txt", "r");

    flockfile(online_users_file);

    while(fgets(user_in_file, 4, online_users_file))
        if(!strcmp(user_in_file, usr_cmp))
            flag = 0;

    funlockfile(online_users_file);
    fclose(online_users_file);

    if(flag)
        return 1;
    else
        return 0;
}

void *req_conn(){
        char sel[5];
        
        printf("Solicitação de conversa recebida! Deseja aceitar? (sim\\nao)");
        fflush(stdout);
        pthread_mutex_lock(&scan_mutex);            
            fgets(sel, sizeof(sel), stdin);  
        pthread_mutex_unlock(&scan_mutex);

        if(!strcmp(sel, "sim\n") || !strcmp(sel, "SIM\n")){
            return (void *) 1;
        }
        else{
            return (void *) 0;
        }   

}

void *new_conn(){
    
}

int msg_arrvd(void *context, char *topic_name, int topic_len, MQTTClient_message *message){
    void ** select;

    if(!strcmp(topic_name, USER_TOPIC_CONTROL)){
        if(!strcmp(message->payload, "conn_req")){

            pthread_create(&req_conn_thread,NULL,req_conn,NULL);

            pthread_join(req_conn_thread, select);

            if(select){
                new_conn(&topic_name);
            } 
        }   

    }

    // printf("     topic: %s\n", topic_name);
    // printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic_name);
    return 1;
}

void conn_lost(void *context, char *cause){
    printf("\nConexão perdida. Erro: %s\n", cause);
}

int pub_msg(MQTTClient client, char * topic, char * payload){
    int rc;

    MQTTClient_message message = MQTTClient_message_initializer;

    message.payload = payload;
    message.payloadlen = sizeof(payload);
    message.qos = QOS;

    if((rc = MQTTClient_publishMessage(client, topic, &message, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to publish message, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(0);
    }

}

void ini_chat(MQTTClient m_client){

    char rec_id[4], rec_topic[12] = "";

    printf("Qual ID do usuario que você deseja mandar mensagem?\n");
    
    __fpurge(stdin);
    fgets(rec_id, sizeof(rec_id), stdin);

    if(ver_usr(rec_id)){
        printf("O usuario não está online\n");
        return;
    }
    else{
        strncat(rec_topic, rec_id, 2);
        strcat(rec_topic, "_Control");
        //printf("%s", rec_topic);
    }
    
    printf("Requisitando inicio de sessão...\n");

    pub_msg(m_client, rec_topic, "conn_req");

}

void *main(){

    char user_in_file[4];
    int sel, rc;    

    printf("Digite seu ID único:\n");
    
    __fpurge(stdin);
    fgets(USER_ID, sizeof(USER_ID), stdin);
    
    online_users_file = fopen("online-users.txt", "a+");

    flockfile(online_users_file);

    while(fgets(user_in_file, 4, online_users_file)){
        if(!strcmp(user_in_file, USER_ID)){
            printf("Você já está online!\n");
            exit(0);
        }
    }

    fputs(USER_ID, online_users_file);

    funlockfile(online_users_file);
    fclose(online_users_file);

    strncat(USER_TOPIC_CONTROL, USER_ID, 2);
    strcat(USER_TOPIC_CONTROL, "_Control");

    strncat(USER_TOPIC_CLIENT, USER_ID, 2);
    strcat(USER_TOPIC_CLIENT, "_Client");

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;


    if((rc = MQTTClient_create(&client_mqtt, ADDRESS, USER_TOPIC_CONTROL, 
    MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Falha na criação do tópico do cliente. Erro: %d\n", rc);
        exit(0);
    }

    //conn_opts.keepAliveInterval = 20;
    //conn_opts.cleansession = 1;
    
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
        pthread_mutex_lock(&scan_mutex);
        menu();
        scanf("%d", &sel);
        pthread_mutex_unlock(&scan_mutex);
        


        switch (sel){
            case 1:
                ini_chat(client_mqtt);
            
            case 2:
                break;

            case 3:
                break;

            case 4:
                logout();
                MQTTClient_destroy(&client_mqtt);
                break;

            default:
                break;
        }
    } while (sel != 4);
    

    
}

