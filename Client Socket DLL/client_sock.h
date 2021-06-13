#pragma once

#ifndef __SHAREDLIB_H
#define __SHAREDLIB_H


#ifdef MATHLIBRARY_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#include "stdlib.h"
extern "C" {
constexpr auto BUF_SIZE = 1024;

	char *sendbuf = (char *)calloc(BUF_SIZE, sizeof(char));
	char *recvbuf = (char *)calloc(BUF_SIZE, sizeof(char));

	EXPORT int Connect(char *ipaddr, char *port);
	EXPORT int Send(int sockfd, const char *message, int size);
	EXPORT int Receive(int sockfd, char *message, int size);
	EXPORT int Close(int sockfd);
}


#endif