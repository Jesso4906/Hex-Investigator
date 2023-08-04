#pragma once
#include "Base.h"
#include <wx/numdlg.h> 

class SavedMenu : public wxFrame, public Base
{
public:
	SavedMenu(HANDLE);

	wxButton* addAddress = nullptr;
	wxGrid* addrList = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;

	HANDLE procHandle;

	std::vector<unsigned long long> addresses;
	std::vector<char> types;
	std::vector<bool> signs;
	std::vector<char> bases;

	void AddAddress(unsigned long long address, char type, bool isSigned, char base);

	void UpdateListWrapper(wxTimerEvent& e);
	template <typename T> void UpdateRow(int row, bool isFloat);

	void WriteValueWrapper(unsigned long long* address, char type, bool isSigned, char base);

	void RemoveRow(int row);

	void AddAddressWrapper(wxCommandEvent& e);

	void RightClickOptions(wxGridEvent& e);

	void CloseWrapper(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};