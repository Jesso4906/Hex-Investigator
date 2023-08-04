#pragma once
#include <any>
#include "Base.h"
#include "TLHelp32.h"
#include <iomanip>
#include <numeric>
#include "ScanSettings.h"
#include "SavedMenu.h"

class Main : public wxFrame, public Base
{
public:
	Main(); // constructor

	wxButton* selectProc = nullptr;
	wxChoice* selectType = nullptr;
	wxChoice* selectScan = nullptr;
	wxButton* firstScan = nullptr;
	wxButton* nextScan = nullptr;

	wxButton* scanSettingsButton = nullptr;
	ScanSettings* scanSettings = nullptr;

	SavedMenu* savedMenu = nullptr;

	wxTextCtrl* valueInput = nullptr;

	wxCheckBox* isSigned = nullptr;

	wxStaticText* baseInputLabel = nullptr;
	wxTextCtrl* baseInput = nullptr;

	wxCheckBox* roundFloats = nullptr;
	wxTextCtrl* roundInput = nullptr;

	wxCheckBox* manualUpdate = nullptr;
	wxButton* manualUpdateButton = nullptr;

	wxGrid* addrList = nullptr;

	wxStaticText* resultsTxt = nullptr;

	wxButton* openSaved = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;

	HANDLE procHandle = 0;

	std::vector<unsigned long long> addressPool;
	std::vector<std::any> oldValues;
	std::vector<unsigned char> allScanValues;

	int base = 10;

	int roundings[8] =
	{
		1,
		10,
		100,
		1000,
		10000,
		100000,
		1000000,
		10000000
	};
	int roundingValue = 1;
	int roundingPlace = 1;

	bool scanning = false;

	void SelectProcess(wxCommandEvent& e);

	void FirstScanWrapper(wxCommandEvent& e);
	template <typename T> void FirstScan(T targetValue);

	void NextScanWrapper(wxCommandEvent& e);
	template <typename T> void NextScan(T targetValue);

	void ResetScan();

	void UpdateListWrapper(wxTimerEvent& e);
	template <typename T> void UpdateList();

	void WriteValueWrapper(unsigned long long* address);

	void RightClickOptions(wxGridEvent& e);

	void RemoveRow(int row);

	void OpenScanSettings(wxCommandEvent& e);

	void OpenSavedMenu(wxCommandEvent& e);

	void UpdateRounding(wxCommandEvent& e);

	void UpdateType(wxCommandEvent& e);

	void UpdateScan(wxCommandEvent& e);

	void ToggleManualUpdate(wxCommandEvent& e);

	void ManualUpdateWrapper(wxCommandEvent& e);

	void CloseWrapper(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};