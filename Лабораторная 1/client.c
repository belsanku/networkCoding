#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define BUFFLEN 255
#define SERVER "127.0.0.1"

int main(int argc, char* argv[]){
    int sock, repeat = 5;
    char buffer[BUFFLEN];
    struct sockaddr_in serverAddr, clientAddr;
    struct hostent *hp, *gethostbyname();
    char *ip = "127.0.0.1";

    if (argc<4){
        printf("Ввести имя_хоста, порт и сообщение.\n");
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Не получен socket");
        exit(1);
    }

    bzero((char*) &serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &serverAddr.sin_addr, hp->h_length);
    serverAddr.sin_port = htons(atoi(argv[2]));

    bzero((char*)&clientAddr, sizeof(clientAddr));

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port = 0;

    // if (bind(sock, &clientAddr, sizeof(clientAddr))){
    //     perror("Клиент не получил порт.");
    //     exit(1);
    // }

    printf("Клиент: Готов к пересылке.\n");
    while(repeat--){
        if (sendto(sock, argv[3], strlen(argv[3]), 0, &serverAddr, sizeof(serverAddr)) < 0){
            perror("Проблемы с отправкой на сервер.");
            exit(1);
        }
     
        bzero(buffer, sizeof(BUFFLEN));
        int slen = sizeof(serverAddr);

        if ((recvfrom(sock, (char*) buffer, BUFFLEN, 0,
            (struct sockaddr*) &serverAddr, &slen)) == -1){
            perror("Неверный сокет сервера.");
            exit(1);
        }

        printf("Возврат: %s\n", buffer);

        sleep(atoi(argv[3]));
    }
    
    printf("Клиент: Пересылка завершена.\n");
    close(sock);
}