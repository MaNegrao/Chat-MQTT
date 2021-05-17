#include "./chat-mqtt.h"

void menu(){
    printf("-- Escolha uma oção --");
    printf("1. Enviar mensagem para um usuario\n");
    printf("2. Enviar mensagem em um grupo \n");
	printf("3. Want to join Group \n");
	printf("4. Want to send Group message \n");
	printf("5. Want to leave Group \n");
	printf("6. Quit Application \n");
}

int main(){

    FILE *users_file;
    char user_in_file[4];    

    printf("Digite seu ID único:\n");
    fgets(USER_ID, sizeof(USER_ID), stdin);

    pthread_mutex_lock(&online_users_mutex);
    users_file = fopen("online-users.txt", "a+");

    while(fgets(user_in_file, 4, users_file)){
        printf("%s", user_in_file);
    }
    

  

    printf("%s", USER_ID);

    pthread_mutex_unlock(&online_users_mutex);
}

