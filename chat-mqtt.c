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
    char user_in_file[64];    

    printf("Digite seu ID único:\n");
    fgets(USER_ID, sizeof(USER_ID), stdin);


    // fpurge();
    pthread_mutex_trylock(online_users_mutex);
    users_file = fopen("online_users.txt", "a+");
        while(fgets(user_in_file, sizeof(user_in_file), users_file)){
            printf("%s\n", user_in_file);
        }

    pthread_mutex_unlock(online_users_mutex);
}

