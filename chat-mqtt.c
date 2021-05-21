#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --\n");
    printf("1. Iniciar um chat com um usuario\n");
    printf("2. Enviar mensagem\n");
    printf("3. Entrar em um grupo \n");
    printf("4. Enviar mensagem em um grupo \n");
	printf("5. Logout \n");
}

void connlost(void *context, char *cause){
    printf("\nConexão perdida. Erro: %s\n", cause);
}

void onDisconnectFailure(void* context, MQTTAsync_failureData* response) {
	printf("Disconnect failed, rc %d\n", response->code);
	disc_finished = 1;
}

void onDisconnect(void* context, MQTTAsync_successData* response) {
	printf("Desconexão bem sucedida!\n");
	disc_finished = 1;
}

void onSendFailure(void* context, MQTTAsync_failureData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	printf("Message send failed token %d error code %d\n", response->token, response->code);
	opts.onSuccess = onDisconnect;
	opts.onFailure = onDisconnectFailure;
	opts.context = client;
	// if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
	// {
	// 	printf("Failed to start disconnect, return code %d\n", rc);
	// 	exit(EXIT_FAILURE);
	// }
}

void onSend(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	//printf("Message with token value %d delivery confirmed\n", response->token);
	opts.onSuccess = onDisconnect;
	opts.onFailure = onDisconnectFailure;
	opts.context = client;
	// if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
	// {
	// 	printf("Failed to start disconnect, return code %d\n", rc);
	// 	exit(EXIT_FAILURE);
	// }
}

void onSubscribe(void* context, MQTTAsync_successData* response) {
	printf("Assinatura Concluida!\n");
	subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response) {
	printf("Falha na assinatura. Erro: %d\n", response->code);
	finished = 1;
}

void onConnect(void* context, MQTTAsync_successData* response) {
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;

	printf("Conexão bem sucedida!\n");
    
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

	if ((rc = MQTTAsync_subscribe(client, USER_TOPIC_CONTROL, QOS, &opts)) != MQTTASYNC_SUCCESS) {
		printf("Falha ao iniciar a assinatura no tópico de controle. Erro: %d\n", rc);
		finished = 1;
	}

    if ((rc = MQTTAsync_subscribe(client, USER_TOPIC_CLIENT, QOS, &opts)) != MQTTASYNC_SUCCESS) {
		printf("Falha ao iniciar a assinatura no tópico de cliente. Erro: %d\n", rc);
		finished = 1;
	}
}

void onConnectFailure(void* context, MQTTAsync_failureData* response) {
	printf("Falha na conexão. Erro: %d\n", response->code);
	finished = 1;
}

void pub_msg(char * topic, char * payload, MQTTAsync client){
    int rc;

    MQTTAsync_message message = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = client;

    message.payload = payload;
    message.payloadlen = (int)strlen(payload);
    message.qos = QOS;

    if((rc = MQTTAsync_sendMessage(client, topic, &message, &opts)) != MQTTASYNC_SUCCESS){
        printf("Falha ao publicar a mesangem. Erro: %d\n", rc);
        MQTTAsync_destroy(client);
        exit(0);
    }

}

void sub_topic(char * topic_sub, MQTTAsync client){
    int rc;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = client;

    if((rc = MQTTAsync_subscribe(client, topic_sub, QOS, &opts)) != MQTTASYNC_SUCCESS){
        printf("Falha ao assinar o tópico, Erro: %d\n", rc);
        MQTTAsync_destroy(&client);
        exit(0);
    }               
}

void handle_new_chat(char * rec_user, MQTTAsync client){
    char topic_rec[20]="", topic_chat[30]="";
    int session_id = rand() % 1000;
    
    sprintf(topic_rec, "%.2s_Client", rec_user);
    sprintf(topic_chat, "%.2s_%.2s_Chat_%d", USER_ID, rec_user, session_id);

    //printf("%s\n", topic_rec);
    //printf("%s\n", topic_chat);

    sub_topic(topic_chat, client);

    pub_msg(topic_rec, topic_chat, client);

    sprintf(TOPICS_ONLINE[atoi(rec_user)], "%s", topic_chat);

    //printf("%s\n", TOPICS_ONLINE[atoi(rec_user)]);

}

int msgarrvd(void *context, char *topic_name, int topic_len, MQTTAsync_message *message){
    int rc;
    char user_id[5], id_rec[3];
    MQTTAsync client = (MQTTAsync)context;

    if(!strcmp(topic_name, USER_TOPIC_CONTROL)){
        strncpy(user_id, (char *)message->payload, 2);
        printf("%.2s Inicou um chat com você\n", (char *)message->payload);
        handle_new_chat(user_id, client);
    }
    else if(!strcmp(topic_name, USER_TOPIC_CLIENT)){
        printf("Olha o chat iniciando!\n");
        fflush(stdout);
        
        sprintf(id_rec, "%.2s", (char *)message->payload);
        sub_topic((char *)message->payload, client);

        sprintf(TOPICS_ONLINE[atoi(id_rec)], "%s", (char *)message->payload);

        //printf("%s\n", TOPICS_ONLINE[atoi(id_rec)]);     
    }
    else{
        //printf("   Topic: %s\n", topic_name);
        char * idus_rec = strtok((char *)message->payload, ";");
        char * message_rec = strtok(NULL, ";");

        if(strcmp(idus_rec, USER_ID_ID)){
            printf("\nOlha a mensagem!\n");
            printf("User %s: %s\n", idus_rec, message_rec);
            
            sleep(4);
            menu();
        }
    }
    return 1;
}

void send_msg_chat(MQTTAsync client){
    int sel; 
    char msg_topic[20]="", message[64], payload[90];

    printf("Selecione um dos chats para mandar mensagem:\n");
    for(int i = 0; i < 99; i++){
        if(strlen(TOPICS_ONLINE[i]) > 5){
            printf("%d - User %2d\n", i, i);
        }
    }

    scanf("%d", &sel);
    strcpy(msg_topic, TOPICS_ONLINE[sel]);

    printf("Digite a mensagem que deseja enviar:\n");

    __fpurge(stdin);
    
    fgets(message, sizeof(message), stdin);

    sprintf(payload, "US;%s;%s", USER_ID_ID, message);

    pub_msg(msg_topic, payload, client);

}

void ini_chat(MQTTAsync client){

    char rec_id[10], rec_topic[20] = "";

    printf("Qual ID do usuario que você deseja mandar mensagem?\n");
    
    __fpurge(stdin);
    fgets(rec_id, sizeof(rec_id), stdin);

    strncat(rec_topic, rec_id, 2);
    strcat(rec_topic, "_Control");

    printf("Requisitando inicio de sessão...\n");
    pub_msg(rec_topic, USER_ID, client);
}

void send_msg_group(MQTTAsync client){
    int sel; 
    char msg_topic[20]="", message[64], payload[90];

    printf("Selecione um dos chats para mandar mensagem:\n");
    for(int i = 1; i < group_control; i++){
        printf("%d - %s\n", i, GP_TOPICS_ONLINE[i]);
    }

    scanf("%d", &sel);
    strcpy(msg_topic, GP_TOPICS_ONLINE[sel]);

    printf("Digite a mensagem que deseja enviar:\n");

    __fpurge(stdin);
    
    fgets(message, sizeof(message), stdin);

    sprintf(payload, "GP;%s;%s", USER_ID_ID, message);

    pub_msg(msg_topic, payload, client);
}

void sub_group(MQTTAsync client){
    int rc;
    char group[20], group_in_file[2];

    printf("Em qual dos grupos abaixo você deseja entrar?\n");
    
    __fpurge(stdin);
    fgets(group, sizeof(group), stdin);

    char * topic = strtok(group, "\n");

    strcat(topic, "_Group");

    strcpy(GP_TOPICS_ONLINE[group_control], topic);

    group_control++;

    sub_topic(topic, client);
}

int main(){
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    int sel, rc;    

    printf("Digite seu ID único:\n");
    
    __fpurge(stdin);
    fgets(USER_ID, sizeof(USER_ID), stdin);

    strncat(USER_ID_ID, USER_ID, 2);

    strncat(USER_TOPIC_CONTROL, USER_ID, 2);
    strcat(USER_TOPIC_CONTROL, "_Control");

    strncat(USER_TOPIC_CLIENT, USER_ID, 2);
    strcat(USER_TOPIC_CLIENT, "_Client");

    if ((rc = MQTTAsync_create(&client, ADDRESS, USER_ID_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL))
			!= MQTTASYNC_SUCCESS)
	{
		printf("Failed to create client, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto exit;
	}

	if ((rc = MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to set callbacks, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start connect, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
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
                ini_chat(client);
                break;
            
            case 2:
                send_msg_chat(client);
                break;

            case 3:
                sub_group(client);
                break;

            case 4:
                send_msg_group(client);
                break;

            case 5:
                disc_opts.onSuccess = onDisconnect;
                disc_opts.onFailure = onDisconnectFailure;
                if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS) {
                    printf("Failed to start disconnect, return code %d\n", rc);
                    rc = EXIT_FAILURE;
                    MQTTAsync_destroy(&client);
                }
                break;

            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (sel != 5);  


destroy_exit:
	MQTTAsync_destroy(&client);
exit:
 	return rc;
}

