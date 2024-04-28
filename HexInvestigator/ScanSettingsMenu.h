#pragma once
#include <wx/wx.h>
#include <sstream>
#include "TLHelp32.h"

class ScanSettingsMenu : public wxFrame
{
public:
	ScanSettingsMenu(HANDLE hProc);

	wxStaticText* protectionLabel = nullptr;
	wxCheckListBox* protectChoice = nullptr;

	wxStaticText* typeLabel = nullptr;
	wxCheckListBox* typeChoice = nullptr;

	wxCheckBox* alignMemory = nullptr;

	wxCheckBox* freezeProcess = nullptr;

	wxStaticText* minAddrTxt = nullptr;
	wxTextCtrl* minAddrInput = nullptr;

	wxStaticText* maxAddrTxt = nullptr;
	wxTextCtrl* maxAddrInput = nullptr;

	wxStaticText* moduleSelectTxt = nullptr;
	wxChoice* moduleSelect = nullptr;

	wxBoxSizer* column1Sizer = nullptr;
	wxBoxSizer* column2Sizer = nullptr;
	wxBoxSizer* column3Sizer = nullptr;
	wxBoxSizer* hSizer = nullptr;

	HANDLE procHandle;

	enum ids
	{
		MainWindowID,
		ProtectionsListID,
		MemoryTypeListID,
		MinAddressInputID,
		MaxAddressInputID,
		SelectModuleID
	};

	int protections[6] =
	{
		PAGE_EXECUTE_READ,
		PAGE_EXECUTE_READWRITE,
		PAGE_EXECUTE_WRITECOPY,
		PAGE_READONLY,
		PAGE_READWRITE,
		-1 // all
	};

	const char* protectStrs[6] =
	{
		"PAGE_EXECUTE_READ",
		"PAGE_EXECUTE_READWRITE",
		"PAGE_EXECUTE_WRITECOPY",
		"PAGE_READONLY",
		"PAGE_READWRITE",
		"ALL"
	};

	const char* typeStrs[3] =
	{
		"MEM_IMAGE",
		"MEM_MAPPED",
		"MEM_PRIVATE"
	};

	int currentProtection = -1;

	bool image = true;
	bool mapped = false;
	bool priv = true;

	unsigned long long minAddress = 0;
	unsigned long long maxAddress = 0x7fffffffffff;

	struct ModuleBounds 
	{
		unsigned long long start, end;
	};
	std::vector<ModuleBounds> moduleAddresses;

	void UpdateProtection(wxCommandEvent& e);
	void DeselectProtection(wxCommandEvent& e);

	void UpdateType(wxCommandEvent& e);
	void DeselectType(wxCommandEvent& e);

	bool UpdateMin();
	bool UpdateMax();

	void UpdateMinOnTextEnter(wxCommandEvent& e);
	void UpdateMaxOnTextEnter(wxCommandEvent& e);
	void UpdateSelectedModule(wxCommandEvent& e);
	void UpdateModuleAddresses();

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};