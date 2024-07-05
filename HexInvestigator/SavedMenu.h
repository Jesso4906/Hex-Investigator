#pragma once
#include "Utils.h"
#include <sstream>
#include <wx/numdlg.h> 
#include <wx/grid.h>
#include "WriteMenu.h"

class SavedMenu : public wxFrame, public Utils
{
public:
	SavedMenu(HANDLE, WriteMenu*);

	wxButton* addAddress = nullptr;
	wxGrid* addrList = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;

	WriteMenu* writeMenu = nullptr;

	enum ids
	{
		MainWindowID,
		AddAddressID,
		AddressListID,
		UpdateTimerID
	};

	HANDLE procHandle;

	struct SavedEntry
	{
		uintptr_t address;
		ValueType type;
		char base;

		int byteArraySize;
	};

	std::vector<SavedEntry> savedEntries;

	void AddAddress(SavedEntry savedEntry);

	void UpdateListOnTimer(wxTimerEvent& e);
	template <typename T> void UpdateRow(int row, bool isFloat);
	void UpdateRowByteArray(int row, int size, bool ascii);

	void WriteValueHandler(wxString input, uintptr_t* address, ValueType type, char base);

	void RemoveRow(int row);

	void AddAddressButtonPress(wxCommandEvent& e);

	void RightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};