# OscSenderReceiver 

A simple lightweight Osc Library base on https://github.com/hyblocker/hekky-osc.git.

### Motivation

An lightweight Osc library for communicating with Ardour DAW and potentially other DAW's.
Supposed to work on Win/Lin/Mac and STM32 microcontroller
Simple, expandable, fast
Groundwork for future development of DAW controllers.
I believe Osc is supposed to be the way how to control DAW's...

### License

This project is licensed under the MIT license. hekky-osc is also licensed under the MIT license.

---

# Example

```cpp
#include "OscMessage.hpp"
#include "OscSenderReceiver.hpp"


int main() {

	//Init osc controller with ardour
	OscSenderReceiver* ardour = new OscSenderReceiver("127.0.0.1", 20, 3819);

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

```

## Supported platforms

| Platform | Supported |
| -------- | --------- |
| Windows  | ✅         |
| MacOS    | ✅         |
| Linux    | ✅         |         |
| STM32    | ✅         |         |
| Rasperry | ✅         |         |

## Goal

This library aims to provide a simple and easy to use API for using OSC. It aims to conform to the entire OSC 1.0 specification. (Not yet achieved)

| Feature                                         | Supported |
| ----------------------------------------------- | --------- |
| Sending OSC messages                            | ✅         |
| Receiving OSC messages                          | ✅         |
| Sending primitive data types (int, float, etc.) | ✅         |
| 32-bit RGBA color                               | ❌         |
| OSC Timetag                                     | ❌         |
| MIDI                                            | ✅         |
| 32-bit RGBA color                               | ❌         |
| Null                                            | ❌         |
| Arrays                                          | ❌         |
| Bundles                                         | ❌         |
| ASCII Character                                 | ❌         |
