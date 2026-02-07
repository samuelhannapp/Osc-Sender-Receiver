#include "oscmessage.hpp"
#include <math.h>
#include <string.h>
#include "oscmessage.hpp"
#include "UdpSenderReceiver.hpp"


OscMessage::OscMessage(const std::string& address)
	: m_address(address), m_readonly(false)
{
	//HEKKYOSC_ASSERT(address.length() > 1, "The address is invalid!");
	//HEKKYOSC_ASSERT(address[0] == '/', "The address is invalid! It should start with a '/'!");
	m_data.reserve(OSC_MINIMUM_PACKET_BYTES);
	this->FormatOscMessage();
}

OscMessage::OscMessage(char* buffer, int buffer_length)
{
	m_address = std::string(buffer);
	//HEKKYOSC_ASSERT(m_address.length() > 1, "The address is invalid!");
	//HEKKYOSC_ASSERT(m_address.at(0) == '/', "The address is invalid! It should start with a '/'!");
	m_type = get_type_list(buffer, buffer_length);
	m_data = initialize_data(buffer, buffer_length);
	m_message_formated = initialize_message(buffer, buffer_length);
	m_readonly = false;

}

OscMessage::~OscMessage() {
	m_data.clear();
}

OscMessage OscMessage::PushString(std::string data) {
	//HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

	std::copy(data.begin(), data.end(), std::back_inserter(m_data));
	m_data.insert(m_data.end(), GetAlignedStringLength(data) - data.length(), 0);
	m_type += "s";
	this->FormatOscMessage();
	return *this;
}

// Aliases
OscMessage OscMessage::PushFloat(float data) {
	if (isinf(data)) {
		m_type += "I";
	}
	else {
		union {
			float f;
			char c[4];
		} primitiveLiteral = { data };

		if (OscMessage::IsLittleEndian()) {
			primitiveLiteral.f = SwapFloat32(data);
		}

		m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 4);
		m_type += "f";
	}
	this->FormatOscMessage();
	return *this;
}
OscMessage OscMessage::PushDouble(double data) {
	if (isinf(data)) {
		m_type += "I";
	}
	else {
		union {
			double d;
			char c[8];
		} primitiveLiteral = { data };

		if (IsLittleEndian()) {
			primitiveLiteral.d = SwapFloat64(data);
		}

		m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 8);
		m_type += "d";
	}
	this->FormatOscMessage();
	return *this;
}
OscMessage OscMessage::PushInt(int data) {
	union {
		int i;
		char c[4];
	} primitiveLiteral = { data };

	if (IsLittleEndian()) {
		primitiveLiteral.i = SwapInt32(data);
	}

	m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 4);
	m_type += "i";
	this->FormatOscMessage();
	return *this;
}
OscMessage OscMessage::PushLongLong(long long data) {
	union {
		long long i;
		char c[8];
	} primitiveLiteral = { data };

	if (IsLittleEndian()) {
		primitiveLiteral.i = SwapInt64(data);
	}

	m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 8);
	m_type += "h";
	this->FormatOscMessage();
	return *this;
}

// Internal function
//only after this function is the /adress included in the data,
//this is a pretty big kind of flaw...
void OscMessage::FormatOscMessage() {
	if (m_message_formated.size())
		m_message_formated.erase(m_message_formated.begin(), m_message_formated.end());

	// Append address
	std::copy(m_address.begin(), m_address.end(), std::back_inserter(m_message_formated));
	m_message_formated.insert(m_message_formated.end(), GetAlignedStringLength(m_address) - m_address.length(), 0);

	// Append types
	std::string temp_m_type(m_type);
	temp_m_type.insert(0, 1, ',');
	std::copy(temp_m_type.begin(), temp_m_type.end(), std::back_inserter(m_message_formated));
	m_message_formated.insert(m_message_formated.end(), GetAlignedStringLength(temp_m_type) - temp_m_type.length(), 0);

	//data is already formated according to the Osc protocol
	std::copy(m_data.begin(), m_data.end(), std::back_inserter(m_message_formated));
	//m_message_formated.insert(m_message_formated.end(), GetAlignedStringLength(m_data) - m_data.length(), 0);
	//here it's not needed... because the data is already formated

}

char* OscMessage::GetBytes(int& size) {
	if (!m_message_formated.size())
		this->FormatOscMessage();


	// Lock this packet
	m_readonly = true;
	size = static_cast<int>(m_message_formated.size());
	return m_message_formated.data();
}

std::string OscMessage::get_type_list(char* buffer, int buffer_length){
	int ctr = 0;
	std::string ret;
	while (ctr++ < buffer_length) {
		if (*buffer++ == ',')
			break;
	}
	while (ctr++ < buffer_length) {
		if (*buffer != '\0')
			ret.push_back(*buffer++);
		else
			break;
	}
	return ret;
}


//that's the whole raw message
std::vector<char> OscMessage::initialize_message(char* buffer, int buffer_length){
	std::vector<char> out;
	for(int i = 0; i < buffer_length; i++)
		out.push_back(*(buffer + i));
	return out;
}

//that's the message only from the data on
std::vector<char> OscMessage::initialize_data(char* buffer, int buffer_length){
	std::vector<char> out;
	int data_start_point = get_data_start_point(buffer, buffer_length);
	
	for(int i = data_start_point; i < buffer_length; i++)
		out.push_back(*(buffer + i));
	return out;
}

int OscMessage::get_data_start_point(){
		int i = 0;
		while (this->m_message_formated[i] != ',')
			i++;
		while (this->m_message_formated[i] != '\0')
			i++;
		//we found the closing '\0' after the type string, now lets go one further...
		i++;
		while ((i % 4) != 0)
			i++;
		return i;
}

	int OscMessage::get_data_start_point(char* buffer, int buffer_length){
		int i = 0;
		while (buffer[i] != ',')
			i++;
		while (buffer[i] != '\0')
			i++;
		//we found the closing '\0' after the type string, now lets go one further...
		i++;
		while ((i % 4) != 0)
			i++;
		return i;
}

int OscMessage::get_string_length(int start_point){
	size_t size = this->m_message_formated.size();
	while(this->m_message_formated.at(start_point) != '\0')
		start_point++;
	//we found the closing '\0', now let's go one further
	start_point++;
	//now make it even to 4 bytes
	while (start_point % sizeof(int) != 0)
		start_point++;
	return start_point;
}

int OscMessage::get_argument_start_point(int argument_nr){

	int start_point = this->get_data_start_point();
	for(int i = 0; i < argument_nr; i++)
		switch(this->m_type.at(i)){
		case 'i':
			start_point += 4;
			break;
		case 'f':
			start_point += 4;
			break;
		case 's':
			start_point = this->get_string_length(start_point);
			break;
		case 'd':
			start_point += 8;
			break;
		case 'h':
			start_point += 8;
			break;
		}
	return start_point;
}

float OscMessage::get_float(int argument_nr){
	int argument_start_point = this->get_argument_start_point(argument_nr);

	unsigned char byte_array[4];
	for (int i = 0; i < 4; i++)
	{
		byte_array[4 - (i + 1)] = this->m_message_formated[argument_start_point + i];
	}

	float ret = 0;
	memcpy(&ret, byte_array, sizeof(float));
	return ret;
}

int OscMessage::get_int(int argument_nr)
{
	int argument_start_point = this->get_argument_start_point(argument_nr);
	int ret = 0;

	for (int i = 0; i < 4; i++) {
		ret |= (((unsigned char)this->m_message_formated[argument_start_point++]) << (24 - (i * 8)));
	}

	return ret;
}

long long OscMessage::get_long_long(int argument_nr)
{
	int argument_start_point = this->get_argument_start_point(argument_nr);
	long long ret = 0;

	for (int i = 0; i < 8; i++) {
		long long byte = (unsigned char)this->m_message_formated[argument_start_point++];
		ret |= byte << (56 - (i * 8));
	}

	return ret;
}

double OscMessage::get_double(int argument_nr){
	int argument_start_point = this->get_argument_start_point(argument_nr);

	unsigned char byte_array[8];
	for (int i = 0; i < 8; i++)
	{
		byte_array[8 - (i + 1)] = this->m_message_formated[argument_start_point + i];
	}

	double val = 0;
	memcpy(&val, byte_array, sizeof(double));
	return val;
}

std::string OscMessage::get_string(int argument_nr){
	int argument_start_point = this->get_argument_start_point(argument_nr);

	std::string ret;
	while (this->m_message_formated[argument_start_point] != '\0')
		ret.push_back(this->m_message_formated[argument_start_point++]);
	return ret;
}

uint64_t OscMessage::GetAlignedStringLength(const std::string& string) {
		uint64_t len = string.length() + (4 - string.length() % 4);
		if (len <= string.length()) len += 4;
		return len;
}

uint64_t OscMessage::GetAlignedStringLength(const std::wstring& string) {
	uint64_t len = string.length() + (4 - string.length() % 4);
	if (len <= string.length()) len += 4;
	return len;
}

bool OscMessage::IsLittleEndian() {
	union {
		uint32_t i;
		char c[4];
	} endianCheck = { 0x01020304 };

	return endianCheck.c[0] != 1;
}

uint32_t OscMessage::SwapInt32(uint32_t num) {
	return static_cast<std::uint32_t>((num << 24) | ((num << 8) & 0x00FF0000) | ((num >> 8) & 0x0000FF00) | (num >> 24));
}

float OscMessage::SwapFloat32(float num) {

	union {
		float f;
		uint32_t ui32;
	} swapper = { num };

	swapper.ui32 = SwapInt32(swapper.ui32);
	return swapper.f;
}

uint64_t OscMessage::SwapInt64(uint64_t num) {
	num = (num & 0x00000000FFFFFFFF) << 32	| (num & 0xFFFFFFFF00000000) >> 32;
	num = (num & 0x0000FFFF0000FFFF) << 16	| (num & 0xFFFF0000FFFF0000) >> 16;
	num = (num & 0x00FF00FF00FF00FF) << 8	| (num & 0xFF00FF00FF00FF00) >> 8;
	return num;
}

double OscMessage::SwapFloat64(double num) {

	union {
		double d;
		uint64_t ui64;
	} swapper = { num };

	swapper.ui64 = SwapInt64(swapper.ui64);
	return swapper.d;
}

