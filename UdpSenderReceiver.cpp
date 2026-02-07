#include "UdpSenderReceiver.hpp"
#include <iostream>

void UdpSenderReceiver::send_data(char *data, int size)
{
	if (size < 1)
		return;
	#ifdef __linux__
	sendto(m_nativeSocket, data, size, 0, (struct sockaddr*)&m_destinationAddress, sizeof(m_destinationAddress)); //thats exactly the same, we should get rid of that one
	#endif
	#ifdef _WIN64
	sendto(m_nativeSocket, data, size, 0, (sockaddr*)&m_destinationAddress, sizeof(m_destinationAddress));
	#endif
	#ifdef __STM32F7xx_HAL_H
	if (size < 1)
		return;
	sendto(m_nativeSocket, data, size, 0, &m_destinationAddress, sizeof(m_destinationAddress));
	#endif
}


#ifdef __linux__
UdpSenderReceiver::UdpSenderReceiver(std::string destination_ip_address, unsigned int udp_port_in, unsigned int udp_port_out)
{
	struct hostent* h;
	//check ip adress
	h = gethostbyname(destination_ip_address.c_str());
	if (h == NULL) {
		std::cout << ("Invalid IP Address!");
		return;
		//exit (EXIT_FAILURE);
	}
	m_destinationAddress.sin_family = h->h_addrtype;
	memcpy((char*)&m_destinationAddress.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	m_destinationAddress.sin_port = htons(udp_port_out);
	// Open the network socket
	m_nativeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_nativeSocket < 0) {
		std::cout << ("Cannot open Socket!");
		return;
		//exit (EXIT_FAILURE);
	}
	//Bind network socket
	sockaddr_in m_localAddress;
	m_localAddress.sin_family = AF_INET;
	m_localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	m_localAddress.sin_port = htons(udp_port_in);
	int result = bind(m_nativeSocket, (struct sockaddr*)&m_localAddress, sizeof(m_localAddress));
	if (result < 0) {
		std::cout << ("Failed to bind to network socket!");
		return;
		//exit (EXIT_FAILURE);
	}
}
#endif

#ifdef _WIN64
UdpSenderReceiver::UdpSenderReceiver(std::string destination_ip_address, unsigned int udp_port_in, unsigned int udp_port_out)
{
			int result = 0;
				WSADATA wsaData;
				// @TODO: Skip if other connections are open
				result = WSAStartup(MAKEWORD(2, 2), &wsaData);
				if (result != 0) {
					std::cout << "WSAStartup failed";
					return;
				}

			// Get localhost as a native network address
			sockaddr_in m_localAddress;
			m_localAddress.sin_family = AF_INET;
			m_localAddress.sin_addr.s_addr = INADDR_ANY;
			m_localAddress.sin_port = htons(udp_port_in);

			// Get the destination as a native network address
			m_destinationAddress.sin_family = AF_INET;
			result = inet_pton(AF_INET, destination_ip_address.c_str(), &m_destinationAddress.sin_addr.s_addr);
			if (result == 0) {
				std::cout << "Invalid IP Address!";
				return;
			}
			else if (result == -1) {
				int errorCode = WSAGetLastError();
				std::cout << std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton#return-value.\n";
				std::cout << "Failed to set IP Address!";
				return;
			}
			m_destinationAddress.sin_port = htons(udp_port_out);

			// Open the network socket
			m_nativeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (m_nativeSocket == INVALID_SOCKET) {
				std::cout << "Failed to create network socket!";
				return;
			}
			result = bind(m_nativeSocket, (sockaddr*)&m_localAddress, sizeof(m_localAddress));
			if (result == SOCKET_ERROR) {
				std::cout << "Failed to bind to network socket!";
				return;
			}
}
#endif

#ifdef __STM32F7xx_HAL_H
udp_sender_receiver::udp_sender_receiver(std::string destination_ip_address, unsigned int udp_port_in, unsigned int udp_port_out)
{
	err_t err;

	/* 1. Create a new UDP control block  */
	m_nativeSocket = udp_new();

	size_t position = 0;
	size_t start = 0;
	std::string ip_1, ip_2, ip_3, ip_4;
	position = destination_ip_address.find('.', position);
	ip_1 = destination_ip_address.substr(start, position - start);
	position++;
	start = position;
	position = destination_ip_address.find('.', position);
	ip_2 = destination_ip_address.substr(start, position - start);
	position++;
	start = position;
	position = destination_ip_address.find('.', position);
	ip_3 = destination_ip_address.substr(start, position - start);
	position++;
	start = position;
	ip_4 = destination_ip_address.substr(start, destination_ip_address.size() - start);

	int ip_1_int = std::stoi(ip_1);
	int ip_2_int = std::stoi(ip_2);
	int ip_3_int = std::stoi(ip_3);
	int ip_4_int = std::stoi(ip_4);

	/* Bind the block to module's IP and port */
	ip_addr_t myIPaddr;
	IP_ADDR4(&myIPaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
	udp_bind(m_nativeSocket, &myIPaddr, udp_port_in);


	/* configure destination IP address and port */
	ip_addr_t DestIPaddr;
	IP_ADDR4(&DestIPaddr, ip_1_int, ip_2_int, ip_3_int, ip_4_int);
	err = udp_connect(m_nativeSocket, &DestIPaddr, udp_port_out);

	if (err == ERR_OK)
	{
		/*Set a receive callback for the upcb */
		//I don't want this to be here...
		udp_recv(m_nativeSocket, udp_receive_callback, NULL);
	}
}
#endif

#ifdef __linux__
//res in linux has to be tested, I accidentially used the buffer_length before, 
//wich is the max size of the given buffer, not the actual size returned...
//I mean it still worked, but I used everytime more space
int UdpSenderReceiver::receive_data(char *buffer)	
{
	struct sockaddr_in sender_address;
	int sender_address_size = sizeof(sender_address);
	int buffer_length = 1024;
	int res = recvfrom(m_nativeSocket, buffer, buffer_length, 0, (struct sockaddr*)&sender_address, (socklen_t*)&sender_address_size);
	return res;
}
#endif

#ifdef _WIN64
int UdpSenderReceiver::receive_data(char *buffer)	
{
	struct sockaddr_in sender_address;
	int sender_address_size = sizeof(sender_address);
	int buffer_length = 1024;
	int res = recvfrom(m_nativeSocket, buffer, buffer_length, 0, (SOCKADDR*)&sender_address, &sender_address_size);
	return res;
}
#endif

#ifdef __STM32F7xx_HAL_H
int osc_controller::udp_sender_receiver::receive_udp_data_raw(char *buffer)
{
	ip_addr_t sender_address;
	int sender_address_size = sizeof(sender_address);
	int buffer_length = 1024;
	int res = recvfrom(m_nativeSocket, buffer, buffer_length, 0, &sender_address, &sender_address_size);
	return res;
}
#endif
