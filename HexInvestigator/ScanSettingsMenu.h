#pragma once
#include <wx/wx.h>
#include <sstream>

class ScanSettingsMenu : public wxFrame
{
public:
	ScanSettingsMenu(wxWindow*);

	wxStaticText* protectionLabel = nullptr;
	wxCheckListBox* protectChoice = nullptr;

	wxStaticText* typeLabel = nullptr;
	wxCheckListBox* typeChoice = nullptr;

	wxCheckBox* alignMemory = nullptr;

	wxStaticText* minAddrTxt = nullptr;
	wxTextCtrl* minAddrInput = nullptr;

	wxStaticText* maxAddrTxt = nullptr;
	wxTextCtrl* maxAddrInput = nullptr;

	wxBoxSizer* column1Sizer = nullptr;
	wxBoxSizer* column2Sizer = nullptr;
	wxBoxSizer* column3Sizer = nullptr;
	wxBoxSizer* column4Sizer = nullptr;
	wxBoxSizer* hSizer = nullptr;

	enum ids
	{
		MainWindowID,
		ProtectionsListID,
		MemoryTypeListID,
		MinAddressInputID,
		MaxAddressInputID
	};

	int protections[5] =
	{
		PAGE_EXECUTE_READ,
		PAGE_EXECUTE_READWRITE,
		PAGE_EXECUTE_WRITECOPY,
		PAGE_READONLY,
		PAGE_READWRITE
	};

	const char* protectStrs[5] =
	{
		"PAGE_EXECUTE_READ",
		"PAGE_EXECUTE_READWRITE",
		"PAGE_EXECUTE_WRITECOPY",
		"PAGE_READONLY",
		"PAGE_READWRITE"
	};

	const char* typeStrs[3] =
	{
		"MEM_IMAGE",
		"MEM_MAPPED",
		"MEM_PRIVATE"
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

	bool UpdateMin();
	bool UpdateMax();

	void UpdateMinOnTextEnter(wxCommandEvent& e);
	void UpdateMaxOnTextEnter(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};