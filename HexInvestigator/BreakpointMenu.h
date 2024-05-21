#pragma once
#include "Utils.h"
#include <bitset>
#include <sstream>
#include <wx/numdlg.h> 
#include <wx/grid.h>
#include "TLHelp32.h"
#include "Ntstatus.h"

class DebugThread; // forward decleration so BreakpointMenu doesn't throw errors

class BreakpointMenu : public wxFrame, public Utils
{
public:
	BreakpointMenu(HANDLE proc);

	wxTextCtrl* addressInput = nullptr;
	wxChoice* selectSize = nullptr;

	wxButton* attachDebugger = nullptr;
	wxButton* detachDebugger = nullptr;
	wxButton* removeHBP = nullptr;
	wxButton* shbpWrite = nullptr; // shbp = set harware breakpoint
	wxButton* shbpReadWrite = nullptr;
	wxButton* shbpExecute = nullptr;

	wxGrid* addrList = nullptr;

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
		uintptr_t address = 0;
		unsigned int hits = 0;
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

	BreakpointMenu* breakpointMenu = nullptr;
	HANDLE procHandle;
};