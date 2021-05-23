#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#include "../protobuf/coordinate.pb-c.h"

#define MAXLINE 1024 /*max text line length*/

int main(int argc, char **argv)
{
    int port_number;
    char *temp = malloc(sizeof(argv[1]) * strlen(argv[1]));
    strcpy(temp, argv[1]);

    if (argc != 3)
    {
        perror("Please specify server port number and ip address!\n");
        return -1;
    }

    if ((port_number = atoi(argv[2])) == 0)
    {
        perror(" Please input port number\n");
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
    int byte_sent, byte_received;
    Coordinate *opponentPos;
    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(temp);
    servaddr.sin_port = htons(port_number); //convert to big-endian order

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(3);
    }

    while (1)
    {
        Coordinate coord;
        coordinate__init(&coord);
        scanf("%d %lf %lf", &coord.id, &coord.x, &coord.y);
        printf("%d %lf %lf\n", coord.id, coord.x, coord.y);
        int size = coordinate__get_packed_size(&coord);
        // printf("Size: %d\n", size);
        byte_sent = send(sockfd, &size, sizeof(int), 0);
        // printf("Size: %d\n", byte_sent);

        void *buffer = malloc(MAXLINE);
        coordinate__pack(&coord, buffer);
        // printf("Abc");
        byte_sent = send(sockfd, buffer, size, 0);
        // printf("eee");

        byte_received = recv(sockfd, &size, sizeof(int), 0);
        printf("Size: %d\n", size);
        if (byte_received <= 0)
        {
            perror("Cant not receive size\n");
            break;
        }
        byte_received = recv(sockfd, buffer, size, 0);
        // printf("%d\n", bytes_received);
        opponentPos = coordinate__unpack(NULL, size, (uint8_t *)buffer);
        // printf("abcd\n");
        printf("The coord received of ID-%d: (%lf, %lf)\n", opponentPos->id, opponentPos->x, opponentPos->y);

        free(buffer);
    }

    close(sockfd);
    exit(0);
}