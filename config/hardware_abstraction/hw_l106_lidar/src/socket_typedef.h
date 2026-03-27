#ifndef __AVOS__SOCKET_TYPEDEFINE_H__
#define __AVOS__SOCKET_TYPEDEFINE_H__

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/file.h>

#define BASE_RESET   "\033[0m"
#define BASE_RED     "\033[31m"      /* Red */

namespace avos {


typedef struct SocketConfig
{
	std::string native_ip;
	int native_port;
	std::string ip;
	int port;
	SocketConfig() {
		native_ip = "";
		native_port = -1;
		ip = "";
		port = -1;
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "ip:" << ip << std::endl;
		std::cout << prefix << "port:" << port << std::endl;
		std::cout << prefix << "native_ip:" << native_ip << std::endl;
		std::cout << prefix << "native_port:" << native_port << std::endl;
	}
} SocketConfig;


typedef struct TcpSocketConfig
{
	std::string ip;
	int port;
	int max_client_num;
	TcpSocketConfig() {
		ip = "";
		port = -1;
		max_client_num = 10;
	}
} TcpSocketConfig;

}
#endif