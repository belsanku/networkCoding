#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define BUFFLEN 255
#define SERVER "127.0.0.1"

int main(){
    int socketMain = 0, msgLength = 0;

    struct sockaddr_in serverAddr, clientAddr;

    char buffer[BUFFLEN];
    char IPbuff[BUFFLEN];
    bzero(IPbuff, sizeof(BUFFLEN));

    if ((socketMain = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Сервер не в состоянии открыть сокет.");
        exit(1);
    }

    bzero((char*)&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER);
    serverAddr.sin_port = 0;

    if (bind(socketMain, (const struct sockaddr* )&serverAddr, sizeof(serverAddr))){
        perror("Связывание сервера прошло неудачно.");
        exit(1);
    }

    socklen_t length = sizeof(serverAddr);

    if (getsockname(socketMain, (struct sockaddr *)&serverAddr, &length)){
        perror("Вызов getsockname неудачен.");
        exit(1);
    }

    inet_ntop(AF_INET, &serverAddr.sin_addr, IPbuff, BUFFLEN);
    printf( "Сервер: номер порта - % d\n\n", ntohs(serverAddr.sin_port ) ) ;
    printf("Сервер: IP адрес сервера: %s\n\n", IPbuff);

    listen(socketMain, 2);

    while(1)
    {
        length = sizeof(clientAddr);
        bzero(buffer, sizeof(BUFFLEN));

        if ((msgLength = recvfrom(socketMain, (char*) buffer, BUFFLEN, 0,
         (struct sockaddr*) &clientAddr, &length)) < 0){
            perror("Неверный сокет клиента.");
            exit(1);
        }
        
        char tmp[BUFFLEN];
        for (int i = 0; i<msgLength; ++i)
        {
            if (buffer[i] != ' ')
            tmp[i] = buffer[i] + 5;
            else break;
        }

        if (sendto(socketMain, (char*) tmp, msgLength, 0, (struct sockaddr*) &clientAddr,
         length) < 0){
            perror("Проблемы с отправкой на клиент.");
            exit(1);
        }
        
        inet_ntop(AF_INET, &clientAddr.sin_addr, IPbuff, BUFFLEN);

        printf( "Сервер: IP адрес клиента: %s\n", IPbuff) ;
        printf( "Сервер: PORT клиента: %d\n", ntohs(clientAddr.sin_port)) ;
        printf( "Сервер: Длина сообщения - %d\n", msgLength);
        printf( "Сервер: Сообщение: %s\n\n", buffer); 

    }
}