#ifndef __AVOS__UDP_CLIENT_H__
#define __AVOS__UDP_CLIENT_H__

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
#include <ifaddrs.h>
#include "socket_typedef.h"

namespace avos {


class UdpClient
{
public:
	UdpClient() {
		socket_fd_ = -1;
	}
	~UdpClient() {
		if (socket_fd_ != -1) {
			CloseConnection();
		}
	}



	std::string getInterfaceNameByIP(const std::string& targetIP) {
		struct ifaddrs *ifaddr, *ifa;
		char host[NI_MAXHOST];
		std::string interfaceName;

		if (getifaddrs(&ifaddr) == -1) {
			perror("getifaddrs");
			return "";
		}

		for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == nullptr) {
				continue;
			}

			int family = ifa->ifa_addr->sa_family;
			if (family == AF_INET || family == AF_INET6) {
				int s = getnameinfo(ifa->ifa_addr,
				                    (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
				                    host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
				if (s != 0) {
					continue;
				}

				if (targetIP == host) {
					interfaceName = ifa->ifa_name;
					break;
				}
			}
		}

		freeifaddrs(ifaddr);
		return interfaceName;
	}

	int Init(const SocketConfig &config) {
		config_ = config;

		if (config_.ip == "" || config_.port < 0) {
			std::cout << "error  config port or ip is error " << std::endl;
			return -1;
		}

		socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);

		if (socket_fd_ < 0)
		{
			std::cout  << "create socket error"  << std::endl;
			return -1;
		}

		int sockflag = 1;
		if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &sockflag, sizeof(int)) < 0)
		{
			std::cout <<  "setsockopt error"  << std::endl;
		}

		// 绑定套接字到本地地址和端口
		struct sockaddr_in local_addr;
		memset(&local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_port = htons(config_.native_port);
		inet_pton(AF_INET, config_.native_ip.c_str(), &local_addr.sin_addr);
		// local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(socket_fd_, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
			perror("bind error");
			std::cout << BASE_RED << "bind error" << BASE_RESET << std::endl;
			return -1;
		}


		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(config_.port);
		server_addr.sin_addr.s_addr = inet_addr(config_.ip.c_str());


		std::string interface_name = getInterfaceNameByIP(config_.native_ip);
		if (interface_name != "") {
			std::cout << "UdpClient bind interface name " << interface_name << "," << config_.native_ip << std::endl;
			const char *interface = interface_name.c_str(); // 替换为你的网卡名称
			if (setsockopt(socket_fd_, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)) < 0) {
				perror("setsockopt SO_BINDTODEVICE failed");
				close(socket_fd_);
				exit(1);
			}
		}

		std::cout << "udp_init ok" << std::endl;
		return 0;
	}
	int RecieveData(char *data, int max_len) {
		int ret = recv(socket_fd_, data, max_len, 0);
		if (ret == 0)
		{
			std::cout << BASE_RED << "target " << config_.ip << ":" << config_.port << " maybe closed" << BASE_RESET << std::endl;
			return 0;
		}
		// struct sockaddr_in  src_addr = {0};
		// int len = sizeof(src_addr);
		// int ret = recvfrom(socket_fd_, data, max_len, 0, (struct sockaddr *)&src_addr, (socklen_t*)&len);
		return ret;

	}
	int SendData(const char *data, int data_len) {
		int ret = sendto(socket_fd_, data, data_len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (ret < 0)
		{
			std::cout <<  "target " << config_.ip << ":" << config_.port << " maybe closed " << ret  << std::endl;
			return 0;
		}
		return ret;
	}
	void CloseConnection() {
		close(socket_fd_);
		socket_fd_ = -1;
	}


private:
	SocketConfig config_;
	int socket_fd_;

	struct sockaddr_in server_addr;



};

}





#endif

