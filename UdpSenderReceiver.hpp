#ifndef UDP_SENDER_RECEIVER
#define UDP_SENDER_RECEIVER
#include <string>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <alsa/asoundlib.h>
#endif

#ifdef _WIN64
// Winsock API
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ws2def.h>
#pragma comment(lib, "Ws2_32.lib")

#include <SDKDDKVer.h>
#define _WINSOCKAPI_
#include <Windows.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#ifdef __STM32F7xx_HAL_H
#include "stm32_sock.h"
#endif

//#include "Defines.hpp"
class UdpSenderReceiver{
public:
	UdpSenderReceiver() {};
	UdpSenderReceiver(std::string destination_ip_address, unsigned int udp_port_in, unsigned int udp_port_out);
	#ifdef __linux__
	int m_nativeSocket;
	sockaddr_in m_destinationAddress;
	#endif
	#ifdef _WIN64
	SOCKET m_nativeSocket;
	sockaddr_in m_destinationAddress;
	#endif
	#ifdef __STM32F7xx_HAL_H
	struct udp_pcb* m_nativeSocket;
	ip_addr_t m_destinationAddress;
	#endif

	void send_data(char *data, int size);
	int receive_data(char *buffer);
};
#endif