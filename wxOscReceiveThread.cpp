#include "wxOscReceiveThread.h"
#include "oscmessage.hpp"

wxOscReceiveThread::wxOscReceiveThread(wxEvtHandler* handler, OscSenderReceiver* osc_controller_object)
	: wxThread(wxTHREAD_DETACHED), m_handler(handler)
{
	osc_controller = osc_controller_object;
}

//Receiving class setup
//thread = new wxOscReceiveThread(this);
//thread->Run();
//Bind(wxEVT_THREAD, &ReceivingClass...::OnThreadUpdate, this);

wxThread::ExitCode wxOscReceiveThread::Entry()
{
	while (1) {
		OscMessage message(osc_controller->receive_data());

		wxThreadEvent event = wxThreadEvent();
		event.SetPayload(message);
		wxQueueEvent(m_handler, event.Clone());
	}
	return nullptr;
}
/*
wxThread::ExitCode wxOscReceiveThread::Entry()
{
	thread_message message;
	message.midi_message = true;
	while (1) {
		char buffer[1024];
		int size = osc_controller->receive_data(buffer);
		
		OscMessage osc_message(buffer, size);
		if (osc_message.GetTypeList().at(0) == 'i') {
			message.midi_data_size = 3;
			int midi_data = osc_message.get_int(0);
			message.midi_data[0] = unsigned char(midi_data & 0xff);
			message.midi_data[1] = unsigned char((midi_data & 0xff00) >> 8);
			message.midi_data[2] = unsigned char((midi_data & 0xff0000) >> 16);
		}
		if (osc_message.GetTypeList().at(0) == 'h') {
			message.midi_data_size = 120;
			if (osc_message.GetTypeList().size() == 15)
				for (int i = 0; i < 15; i++){
					long long temp = osc_message.get_long_long(i);
					char char_data[8] = { 0 };
					char_data[7] = long long(temp & long long(0xff) << 56) >> 56;
					char_data[6] = long long(temp & long long(0xff) << 48) >> 48;
					char_data[5] = long long(temp & long long(0xff) << 40) >> 40;
					char_data[4] = long long(temp & long long(0xff) << 32) >> 32;
					char_data[3] = (temp & long long(0xff << 24)) >> 24;
					char_data[2] = (temp & long long(0xff << 16)) >> 16;
					char_data[1] = (temp & long long(0xff << 8)) >> 8;
					char_data[0] = (temp & long long(0xff << 0)) >> 0;
					for (int o = 0; o < 8; o++)
						message.midi_data[o + 8 * i] = char_data[o];
				}
		}
		wxThreadEvent event = wxThreadEvent();
		event.SetPayload(message);
		wxQueueEvent(m_handler, event.Clone());
	}
	return nullptr;
}
*/
/*
void ReceivingClass...::OnThreadUpdate(wxThreadEvent& event)
{
	// SAFE: runs on GUI thread
	std::vector<char> data;
	data = event.GetPayload<std::vector<char>>();
	char array[1024];
	for (int i = 0; i < data.size(); i++)
		array[i] = data.at(i);
	OscMessage message(array, data.size());
	int index = message.get_int(0) - 1;
	this->channel_name[index]->SetLabel(message.get_string(1));
	this->channel_name[index]->SetSize(channel_name_panel[index]->GetSize());
}
*/
