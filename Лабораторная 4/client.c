#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    
    int socketClient = 0;
    struct sockaddr_in serverAddress = {};
    struct hostent *hostInf;

	if (argc < 4) {
	    printf("TCP-клиент: Введите имя хоста, порт и сообщение!\n");
	    exit(1);
	}

    if ((socketClient = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Невозможно получить сокет для сервера!\n");
        exit(1);
    }

    hostInf = gethostbyname(argv[1]);

    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    memcpy(&serverAddress.sin_addr, hostInf->h_addr, hostInf->h_length);
    serverAddress.sin_port = htons(atoi(argv[2]));

    if (connect(socketClient, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Клиент: невозможно установить соединение\n");
        exit(1);
    }

    printf("Клиент: установка соединения прошла отлично. Готов к пересылке\n");

    while(true) {
        if (send(socketClient, argv[3], strlen(argv[3]), 0) < 0) {
            perror("Возникли проблемы с пересылкой\n");
            exit(1);
        }
        sleep(atoi(argv[3]));
    }

    printf("Соединение прервано\n");
    close(socketClient);

    return 0;
}