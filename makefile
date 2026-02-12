libosc.a: OscMessage.o OscSenderReceiver.o UdpSenderReceiver.o wxOscReceiveThread.o 
	ar rcs -g libosc.a OscMessage.o OscSenderReceiver.o UdpSenderReceiver.o wxOscReceiveThread.o

OscMessage.o: OscMessage.cpp
	g++ -g -c OscMessage.cpp -I/home/samuel/Osc-Sender-Receiver

OscSenderReceiver.o: OscSenderReceiver.cpp
	g++ -g -c OscSenderReceiver.cpp -I/home/samuel/Osc-Sender-Receiver

UdpSenderReceiver.o: UdpSenderReceiver.cpp
	g++ -g -c UdpSenderReceiver.cpp -I/home/samuel/Osc-Sender-Receiver

wxOscReceiveThread.o: wxOscReceiveThread.cpp
	g++ -g -c wxOscReceiveThread.cpp -I/home/samuel/Osc-Sender-Receiver `wx-config --cxxflags` `wx-config --libs`
