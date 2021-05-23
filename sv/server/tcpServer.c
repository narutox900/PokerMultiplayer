#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>

#include "tcpServer.h"

#define MAXLINE 1024 /*max text line length*/
#define MAX_CLIENT 2

int listenfd, connfd[MAX_CLIENT], n;
pid_t childpid;
socklen_t clilen[MAX_CLIENT];
struct sockaddr_in cliaddr[MAX_CLIENT], servaddr;
int bytes_sent, bytes_received;
PlayerStat players[MAX_CLIENT];

int sendBulletPosition(int clientId)
{
    // BulletSpawn bullet = {.x=players[clientId].x, .y=players[clientId].y,};
    // bullet.x = players[]
    int size = bullet_spawn__get_packed_size();
}

void *handleClient(void *client_id)
{
    int id = *(int *)client_id;
    void *buffer = (void *)malloc(MAXLINE);
    int size;
    while (1)
    {
        // get length
        bytes_received = recv(connfd[id], &size, sizeof(int), 0);
        printf("Size: %d\n", size);
        if (bytes_received <= 0)
        {
            perror("Cant not receive size!\n");
            break;
        }
        bytes_received = recv(connfd[id], buffer, size, 0);
        // printf("%d\n", bytes_received);
        PlayerStat *tmpCoord = coordinate__unpack(NULL, size, (uint8_t *)buffer);
        // printf("abcd\n");
        positions[id] = *tmpCoord;
        // printf("efg\n");
        printf("The coord received: (%lf, %lf)\n", positions[id].x, positions[id].y);

        printf("Forwarding...\n");

        //sending size
        bytes_sent = send(connfd[1 - id], &size, sizeof(int), 0);
        //sending buffer
        bytes_sent = send(connfd[1 - id], buffer, size, 0);
        printf("Forward completed!");
        coordinate__free_unpacked(tmpCoord, NULL);
    }

    free(buffer);
    return NULL;
}

int main(int argc, char **argv)
{
    int port_number;
    if (argc < 2)
    {
        perror("Error! No running port specified!\n");
        return -1;
    }
    if ((port_number = atoi(argv[1])) == 0)
    {
        perror(" Please input port number\n");
        return -1;
    }

    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port_number);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, MAX_CLIENT);

    printf("%s\n", "Server running...waiting for connections.");
    int nConnection = 0;
    pthread_t client_ids[MAX_CLIENT];
    while (1)
    {
        if (nConnection < MAX_CLIENT)
        {
            clilen[nConnection] = sizeof(cliaddr[nConnection]);
            if ((connfd[nConnection] = accept(listenfd, (struct sockaddr *)&cliaddr[nConnection], &clilen[nConnection])) == -1)
                perror("Error accepting connection!\n");
            else
            {
                int *client_id = (int *)malloc(sizeof(int));
                *client_id = nConnection;
                printf("Connecttion established for client %d!\n", nConnection + 1);

                pthread_create(&client_ids[nConnection], NULL, handleClient, client_id);
                nConnection++;
            }
        }
    }

    //close thread
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        pthread_join(client_ids[i], NULL);
    }

    close(listenfd);
}
