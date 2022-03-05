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

int main(int argc, char *argv[])
{
    int sock = 0;
    struct sockaddr_in servAddr = {};
    struct hostent *hp;

    if (argc < 4)
    {
        printf("TCP-Клиент. Введите: имя_хоста, порт, сообщение\n");
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("He могу получить сокет\n");
        exit(1);
    }

    hp = gethostbyname(argv[1]);

    memset((char *)&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Клиент не может соединиться.\n");
        exit(1);
    }

    printf("Клиент: Готов к пересылке\n");
    while (true)
    {
        if (send(sock, argv[3], strlen(argv[3]), 0) < 0)
        {
            perror("Проблемы с пересылкой.\n");
            exit(1);
        }

        sleep(atoi(argv[3]));
    }
    printf("Клиент: Отключён от сервера.\n");
    close(sock);

    return 0;
}