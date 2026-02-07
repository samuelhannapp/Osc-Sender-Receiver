#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include <OscSenderReceiver.hpp>

struct MidiMessage_v2 {
	char midi_data[120];
	int midi_data_size;
};

class wxOscReceiveThread : public wxThread {
public:
	wxOscReceiveThread(wxEvtHandler* handler, OscSenderReceiver* osc_controller_object);
	OscSenderReceiver* osc_controller;
protected:
	ExitCode Entry() override;
	
private:
	wxEvtHandler* m_handler;
};

