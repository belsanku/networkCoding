#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFLEN 255
#define SOCKET_COUNT 255

int main() {

    int socketServer = 0;
    int length = 0;

    struct sockaddr_in serverAddress;

    if ((socketServer = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Серверу не удалось получить сокет\n");
        exit(1);
    }

    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = 0;

    if (bind(socketServer, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
        perror("Связывание сервера прошло неудачно\n");
        exit(1);
    }

    length = sizeof(serverAddress);

    if (getsockname(socketServer, (struct sockaddr *)&serverAddress, (socklen_t *restrict)&length)) {
        perror("Не удалось получить локальный адрес протокола, связанный с сокетом\n");
        exit(1);
    }

    char string_buffer[BUFFLEN] = {'\0'};
    inet_ntop(AF_INET, &serverAddress.sin_addr, string_buffer, BUFFLEN);
    printf("Информация о сервере:\n");
    printf("\tIP: %s\n\tPORT: %d\n", string_buffer, ntohs(serverAddress.sin_port));

    listen(socketServer, 3);

    char buffer[BUFFLEN] = {'\0'};
    fd_set master_rfds, rfds;
    int maximumSocket = socketServer;

    FD_ZERO(&master_rfds);
    FD_SET(socketServer, &master_rfds);

    while(true) {
        rfds = master_rfds;
        if (select(maximumSocket +1, &rfds, NULL, NULL, NULL)) {
            for (int i = 0; i<=maximumSocket; ++i) {
                if (FD_ISSET(i, &rfds)) {
                    if (i == socketServer) {
                        int newFD = 0;
                        if ((newFD = accept(socketServer, NULL, NULL)) < 0) {
                            perror("Не удалось принять соединение\n");
                        }
                        else {
                            FD_SET(newFD, &master_rfds);
                            if (newFD > maximumSocket) {
                                maximumSocket = newFD;
                            }else {
                                maximumSocket = maximumSocket;
                            }
                        }
                    } else {
                        memset((char *)&buffer, 0, sizeof(buffer));
                        int messageLen = recv(i, buffer, BUFFLEN, 0);

                        if (messageLen < 0) {
                            perror("Плохое получение дочерним процессом\n");
                        } else if (messageLen == 0) {
                            printf("Сервер: клиент %d прекратил соединение\n\n", i);
                            close(i);
                            FD_CLR(i, &master_rfds);
                        }else {
                            printf("Сервер: сокет для клиента: %d\n", i);
                            printf("\tДлина сообщения: %d\n", messageLen);
                            printf("\tСообщение: %s\n\n", buffer);
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}