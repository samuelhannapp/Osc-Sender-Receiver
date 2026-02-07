#pragma once
#include "UdpSenderReceiver.hpp"
#include "oscmessage.hpp"
class OscSenderReceiver : public UdpSenderReceiver {
public:
	using UdpSenderReceiver::UdpSenderReceiver;
	void send_data(OscMessage message);
	OscMessage receive_data();
};
