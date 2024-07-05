#pragma once
#include "Utils.h"
#include <sstream>
#include <chrono>
#include <wx/numdlg.h>
#include <wx/grid.h>

class WriteMenu : public wxFrame, public Utils
{
public:
	WriteMenu(HANDLE);

	wxButton* setWriteOperation = nullptr;
	wxGrid* addrList = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* writeTimer = nullptr;

	enum ids
	{
		MainWindowID,
		SetWriteOperationID,
		AddressListID,
		WriteTimerID
	};

	HANDLE procHandle;

	std::chrono::steady_clock::time_point lastTime;

	enum Operation
	{
		Set,
		Add,
		Subtract,
		Multiply,
		Divide
	};

	const char* operationStrs[5] =
	{
		"Set", "Add", "Subtract", "Multiply", "Divide"
	};

	struct WriteEntry 
	{
		uintptr_t address;
		ValueType type;
		Operation operation;

		double value;
		wxString valueStr;
		
		int frequency; // miliseconds
		long timer;
	};

	std::vector<WriteEntry> writeEntries;

	void AddWriteEntry(WriteEntry entry);

	void WriteOnTimer(wxTimerEvent& e);

	void WriteOperationHandler(WriteEntry writeEntry);

	template <typename T> void ExecuteWriteOperation(WriteEntry writeEntry);

	void RemoveRow(int row);

	void RightClickOptions(wxGridEvent& e);

	void AddAddressButtonPress(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};