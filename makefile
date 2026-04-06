libosc.a: OscMessage.o OscSenderReceiver.o UdpSenderReceiver.o wxOscReceiveThread.o 
	ar rcs libosc.a OscMessage.o OscSenderReceiver.o UdpSenderReceiver.o wxOscReceiveThread.o

OscMessage.o: OscMessage.cpp
	g++ -g -c OscMessage.cpp 

OscSenderReceiver.o: OscSenderReceiver.cpp
	g++ -g -c OscSenderReceiver.cpp 

UdpSenderReceiver.o: UdpSenderReceiver.cpp
	g++ -g -c UdpSenderReceiver.cpp 

wxOscReceiveThread.o: wxOscReceiveThread.cpp
	g++ -g -c wxOscReceiveThread.cpp `wx-config --cxxflags` `wx-config --libs`
