#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#define OSC_MINIMUM_PACKET_BYTES  8

struct OscMessage {
public:
	OscMessage(const std::string& address);
	OscMessage(char* buffer, int buffer_length);
	~OscMessage();

	static uint64_t GetAlignedStringLength(const std::string& string);
	static uint64_t GetAlignedStringLength(const std::wstring& string);
	static bool IsLittleEndian();
	static uint32_t SwapInt32(uint32_t num);
	static uint64_t SwapInt64(uint64_t num);
	static double SwapFloat64(double num);
	static float SwapFloat32(float num);

	//why do all of these messages give back a copy of the OscMessage????????????????????????????????
	//that doesn't make sense...
	OscMessage PushString(std::string data);
	OscMessage PushFloat(float data);
	OscMessage PushDouble(double data);
	OscMessage PushInt(int data);
	OscMessage PushLongLong(long long data);

	inline const std::string& GetAddress() const {
		return m_address;
	}
	inline const std::string& GetTypeList() const{
		return m_type;
	}
	inline const std::vector<char>& GetData() const{
		return m_data;
	}

	int get_int(int where);
	float get_float(int where);
	double get_double(int where);
	long long get_long_long(int where);
	std::string get_string(int where);
	std::string initialize_type_list(){return this->m_type;}
	void FormatOscMessage();
	char* GetBytes(int& size);

private:
	std::vector<char> initialize_message(char* buffer, int buffer_length);
	std::vector<char> initialize_data(char* buffer, int buffer_length);
	std::string get_type_list(char* buffer, int buffer_length);

	int get_data_start_point();
	int get_data_start_point(char* buffer, int buffer_length);
	int get_argument_start_point(int where);
	int get_string_length(int where);

	bool m_readonly;
	std::string m_address;
	std::string m_type;
	//m_message_formated is where we get data from and m_data is where we push data to...
	//the thing is, if we would get data from m_data, than we would not have to do the 
	//find starting point...
	 
	//this is the complete message with header(addres, and type_list) appended...
	//this has to be initialised after a push was done...
	//maybe we could do that automatically...
	std::vector<char> m_message_formated;
	//this is just the raw data(already formated) used by the push functions
	std::vector<char> m_data;
};
