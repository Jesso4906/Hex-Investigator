#pragma once
#include "Utils.h"
#include <bitset>
#include <sstream>
#include <wx/numdlg.h> 
#include <wx/grid.h>
#include "TLHelp32.h"
#include "Ntstatus.h"
#include <winternl.h>

class DebugThread; // forward decleration so BreakpointMenu doesn't throw errors

class BreakpointMenu : public wxFrame, public Utils
{
public:
	BreakpointMenu(HANDLE proc);

	wxTextCtrl* addressInput = nullptr;
	wxChoice* selectSize = nullptr;

	wxCheckBox* setBeingDebugged = nullptr;

	wxButton* attachDebugger = nullptr;
	wxButton* detachDebugger = nullptr;
	wxButton* removeHBP = nullptr;
	wxButton* shbpWrite = nullptr; // shbp = set harware breakpoint
	wxButton* shbpReadWrite = nullptr;
	wxButton* shbpExecute = nullptr;

	wxGrid* addrList = nullptr;

	wxStaticText* infoAboutAddresses = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateListTimer = nullptr;

	enum ids
	{
		MainWindowID,
		AttachDebuggerID,
		SHBPWriteID,
		SHBPReadWriteID,
		SHBPExecuteID,
		AddressListID,
		DetachDebuggerID,
		RemoveHBPID,
		UpdateTimerID
	};

	enum BPType
	{
		Write,
		ReadWrite,
		Execute
	};
	enum BPSize
	{
		OneByte,
		TwoByte,
		FourByte,
		EightByte
	};

	const char* sizeStrs[4]
	{
		"1",
		"2",
		"4",
		"8"
	};

	HANDLE procHandle;

	struct ListEntry 
	{
		uintptr_t address;
		wxString moduleName;
		unsigned int hits;
	};
	std::vector<ListEntry> entries;

	uintptr_t currentAddress;
	BPSize currentSize;
	BPType currentType;

	bool isBPSet = false;
	bool isDebuggerAttached = false;

	DebugThread* debugThread = nullptr;

	bool SetHardwareBreakpoint(uintptr_t address, BPSize size, BPType type);
	bool DisableDebugRegisters();

	void AttachDebugger(wxCommandEvent& e);
	void DetachDebugger(wxCommandEvent& e);

	void RemoveHBP(wxCommandEvent& e);

	void SetWriteBP(wxCommandEvent& e);
	void SetReadWriteBP(wxCommandEvent& e);
	void SetExecuteBP(wxCommandEvent& e);

	void AddAddressToList(uintptr_t address);

	void UpdateList(wxTimerEvent& e);

	void ClearList();

	void RightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};

class DebugThread : public wxThread
{
public:
	DebugThread(BreakpointMenu* bpMenu, HANDLE proc);

	// thread execution starts heres
	virtual void* Entry() wxOVERRIDE;

	struct ProcessEnvironmentBlock
	{
		BYTE Reserved1[2];
		BYTE BeingDebugged;
	};

	typedef NTSTATUS(__stdcall* tNtQueryInformationProcess)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

	uintptr_t GetPEBAddress(HANDLE procHandle);

	BreakpointMenu* breakpointMenu = nullptr;
	HANDLE procHandle;
};