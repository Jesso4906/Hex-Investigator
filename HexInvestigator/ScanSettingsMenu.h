#pragma once
#include <wx/wx.h>
#include <sstream>
#include "TLHelp32.h"
#include "Utils.h"

class ScanSettingsMenu : public wxFrame, public Utils
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

	wxCheckBox* displayModuleInfo = nullptr;

	wxStaticText* scanKeybindLabel = nullptr;
	wxChoice* scanKeybindSelect = nullptr;

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
		SelectModuleID,
		SelectKeybindID
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

	uintptr_t minAddress = 0;
	uintptr_t maxAddress = 0x7fffffffffff;

	struct ModuleBounds 
	{
		uintptr_t start, end;
	};
	std::vector<ModuleBounds> moduleAddresses;

	const char* keyStrs[62] =
	{
		"None",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"A", "B", "C", "D","E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Space",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
		"Left Shift", "Right Shift", "Left Control", "Right Control", "Left Alt", "Right Alt", "Tab", "Caps Lock", "Enter", "Backspace", "Escape", "Insert"
	};

	//https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	const int keyCodes[62] =
	{
		-1,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x20,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x09, 0x14, 0x0D, 0x08, 0x1B, 0x2D
	};

	int scanKeybind = -1;

	void UpdateProtection(wxCommandEvent& e);
	void DeselectProtection(wxCommandEvent& e);

	void UpdateType(wxCommandEvent& e);
	void DeselectType(wxCommandEvent& e);

	bool UpdateMinAndMax();

	void UpdateSelectedModule(wxCommandEvent& e);
	void UpdateModuleAddresses();

	void UpdateSelectedKeybind(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};