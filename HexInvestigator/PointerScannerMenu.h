#pragma once
#include <sstream>
#include "Utils.h"
#include <wx/grid.h>
#include <TLHelp32.h>

class PointerScannerMenu : public wxFrame, public Utils 
{
public:
	PointerScannerMenu(HANDLE);

	wxTextCtrl* addressInput = nullptr;

	wxStaticText* levelsInputLabel = nullptr;
	wxTextCtrl* levelsInput = nullptr;

	wxStaticText* maxOffsetInputLabel = nullptr;
	wxTextCtrl* maxOffsetInput = nullptr;

	wxStaticText* resultsTxt = nullptr;

	wxButton* scanButton = nullptr;
	wxGrid* pointerGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID,
		ScanButtonID,
		PointerGridID
	};

	HANDLE procHandle;

	std::vector<uintptr_t> moduleHandles;
	std::vector<wxString> moduleNames;

	std::vector<uintptr_t> baseAddresses;
	std::vector<std::vector<unsigned int>> offsets;

	void ScanButtonPress(wxCommandEvent& e);

	unsigned int PointerScan(uintptr_t address, bool is32Bit, int maxOffset, int levels, std::vector<unsigned int> currentOffsets);

	wxString FormatBaseAddress(uintptr_t baseAddress);

	void UpdateModuleHandles();

	void RightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};