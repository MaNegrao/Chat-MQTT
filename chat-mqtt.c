#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --\n");
    printf("1. Enviar mensagem para um usuario\n");
    printf("2. Enviar mensagem em um grupo \n");
	printf("3. Logout \n");
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

void ini_chat(){

    

    //MQTTClient_subscribe(client_mqtt, USER_TOPIC, QOS);

}

void *main(){

    MQTTClient client_mqtt;
    char user_in_file[4];
    int sel;    

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

    strncat(USER_TOPIC, USER_ID, 2);

    strcat(USER_TOPIC, "_Control");


    MQTTClient_create(client_mqtt, ADDRESS, USER_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions connect_options = MQTTClient_connectOptions_initializer;
   
    MQTTClient_connect(client_mqtt, &connect_options);


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
                break;

            default:
                printf("Opção não suportada!\n");
                break;
        }
    } while (sel != 3);
    

    
}

