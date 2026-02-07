#ifdef __linux__
#include "/home/samuel/Osc-Sender-Receiver/OscMessage.hpp"
#include "/home/samuel/Osc-Sender-Receiver/OscSenderReceiver.hpp"
#endif
#ifdef _WIN64
#include "OscMessage.hpp"
#include "OscSenderReceiver.hpp"
#endif
//example:

int main() {

	//Init osc controller with ardour
	OscSenderReceiver* ardour = new OscSenderReceiver("127.0.0.1", 20, 3819);

	//setup controller type
	OscMessage setup_msg("/set_surface");
	setup_msg.PushInt(8);
	setup_msg.PushInt((1 << 0) | (1 << 1));
	setup_msg.PushInt((1 << 0) | (1 << 1) | (1 << 13)/* | (1 << MeteringasFloat)*/);
	setup_msg.PushInt(0b1000);
	ardour->send_data(setup_msg);

	//build osc message
	OscMessage test_message("/strip/fader");
	test_message.PushInt(2);
	test_message.PushFloat(0.234);
	ardour->send_data(test_message);
	
	//receive osc messages
	while (1) {
		OscMessage receive_message = ardour->receive_data();
		std::string osc_address = receive_message.GetAddress();
		std::string type_list = receive_message.GetTypeList();
		int test_int;
		float test_float;
		std::string test_string;
		if (type_list.size())
			switch (type_list.at(0)) {
			case 'i':
				test_int = receive_message.get_int(0);
				break;
			case 'f':
				test_float = receive_message.get_float(0);
				break;
			case 's':
				test_string = receive_message.get_string(0);
				break;
			}
	}
	
	return 1;
}