#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --\n");
    printf("1. Enviar mensagem para um usuario\n");
    printf("2. Enviar mensagem em um grupo \n");
	printf("3. Logout \n");
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
    printf("\nConexão perdida. Erro: %s\n", cause);

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

void ini_chat(){


}

// void delivered(void *context, MQTTClient_deliveryToken dt)
// {
//     printf("Message with token value %d delivery confirmed\n", dt);
//     deliveredtoken = dt;
// }

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

    MQTTClient client_mqtt;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    if((rc = MQTTClient_create(&client_mqtt, ADDRESS, USER_TOPIC_CONTROL, 
    MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Falha na criação do tópico do cliente. Erro: %d\n", rc);
        exit(0);
    }
    
    //conn_opts.keepAliveInterval = 20;
    //conn_opts.cleansession = 1;
    
    if((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS){
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

    system("clear");
    printf("Bem-vindo! Agora você está online!\n\n");

    do
    {
        menu();
        scanf("%d", &sel);
        
        switch (sel){
            case 1:
                ini_chat();
                break;
            
            case 2:
                break;

            case 3:
                logout();
                MQTTClient_destroy(&client_mqtt);
                break;

            default:
                printf("Opção não suportada!\n");
                break;
        }
    } while (sel != 3);
    

    
}

