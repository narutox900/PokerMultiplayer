#ifndef UNICODE
#define UNICODE
#endif

#include "client_sock.h"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
//#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")



int Connect(char *ipaddr, char *port)
{
	int client_sock;
	struct sockaddr_in server_addr; /* server's address information */

	//Step 1: Construct socket
	client_sock = (int)socket(AF_INET, SOCK_STREAM, 0);

	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ipaddr);

	//Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		return -1;
	}

	u_long mode = 1;  // 1 to enable non-blocking socket
	ioctlsocket(client_sock, FIONBIO, &mode);

	return client_sock;
}

int Send(int sockfd, const char *message, int size)
{
	int n;
	memset(sendbuf, 0, BUF_SIZE);
	memcpy(sendbuf, message, size);
	if ((n = send(sockfd, sendbuf, size, 0)) < 0)
	{
		return -1;
	}
	return n;
}

int Receive(int sockfd, char *message, int size)
{
	int n;
	memset(recvbuf, 0, BUF_SIZE);
	if ((n = recv(sockfd, recvbuf, size, 0)) < 0)
	{
		return -1;
	}
	memcpy(message, recvbuf, size);
	return n;
}

int Close(int sockfd)
{
	closesocket(sockfd);
	return 1;
}
