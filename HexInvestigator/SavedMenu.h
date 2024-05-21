#pragma once
#include "Utils.h"
#include <sstream>
#include <wx/numdlg.h> 
#include <wx/grid.h>

class SavedMenu : public wxFrame, public Utils
{
public:
	SavedMenu(HANDLE);

	wxButton* addAddress = nullptr;
	wxGrid* addrList = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;

	enum ids
	{
		MainWindowID,
		AddAddressID,
		AddressListID,
		UpdateTimerID
	};

	HANDLE procHandle;

	std::vector<uintptr_t> addresses;
	std::vector<char> types;
	std::vector<char> bases;
	std::vector<int> sizes; // for byte arrays

	void AddAddress(uintptr_t address, char type, char base, int size);

	void UpdateListOnTimer(wxTimerEvent& e);
	template <typename T> void UpdateRow(int row, bool isFloat);
	void UpdateRowByteArray(int row, int size);

	void WriteValueHandler(wxString input, uintptr_t* address, char type, char base);

	void RemoveRow(int row);

	void AddAddressButtonPress(wxCommandEvent& e);

	void RightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};