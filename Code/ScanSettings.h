#pragma once
#include <wx/wx.h>
#include <sstream>

class ScanSettings : public wxFrame
{
public:
	ScanSettings(wxWindow*);

	wxStaticText* protectionLabel = nullptr;
	wxCheckListBox* protectChoice = nullptr;

	wxStaticText* typeLabel = nullptr;
	wxCheckListBox* typeChoice = nullptr;

	wxCheckBox* alignMemory = nullptr;
	wxCheckBox* excludeZero = nullptr;

	wxStaticText* minAddrTxt = nullptr;
	wxTextCtrl* minAddrInput = nullptr;

	wxStaticText* maxAddrTxt = nullptr;
	wxTextCtrl* maxAddrInput = nullptr;

	wxBoxSizer* column1Sizer = nullptr;
	wxBoxSizer* column2Sizer = nullptr;
	wxBoxSizer* column3Sizer = nullptr;
	wxBoxSizer* column4Sizer = nullptr;
	wxBoxSizer* hSizer = nullptr;

	int protections[5] =
	{
		PAGE_EXECUTE_READ,
		PAGE_EXECUTE_READWRITE,
		PAGE_EXECUTE_WRITECOPY,
		PAGE_READONLY,
		PAGE_READWRITE
	};

	int currentProtection = PAGE_READWRITE;

	bool image = true;
	bool mapped = false;
	bool priv = true;

	unsigned long long minAddress = 0;
	unsigned long long maxAddress = 0x7fffffffffff;

	void UpdateProtection(wxCommandEvent& e);
	void DeselectProtection(wxCommandEvent& e);

	void UpdateType(wxCommandEvent& e);
	void DeselectType(wxCommandEvent& e);

	void UpdateMinWrapper(wxCommandEvent& e);
	void UpdateMaxWrapper(wxCommandEvent& e);
	bool UpdateMin();
	bool UpdateMax();

	void CloseWrapper(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};