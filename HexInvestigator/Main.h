#pragma once
#include "Utils.h"
#include <wx/grid.h>
#include "TLHelp32.h"
#include <iomanip>
#include <numeric>
#include <sstream>
#include <thread>
#include "ScanSettingsMenu.h"
#include "SavedMenu.h"
#include "BreakpointMenu.h"

class Main : public wxFrame, public Utils
{
public:
	Main(); // constructor

	wxButton* selectProc = nullptr;
	wxChoice* selectValueType = nullptr;
	wxChoice* selectScanType = nullptr;
	wxButton* firstScan = nullptr;
	wxButton* nextScan = nullptr;

	wxButton* scanSettingsButton = nullptr;
	ScanSettingsMenu* scanSettingsMenu = nullptr;

	SavedMenu* savedMenu = nullptr;
	BreakpointMenu* breakpointMenu = nullptr;

	wxButton* openSaved = nullptr;
	wxButton* openBreakpointer = nullptr;

	wxTextCtrl* valueInput = nullptr;

	wxStaticText* baseInputLabel = nullptr;
	wxTextCtrl* baseInput = nullptr;

	wxCheckBox* roundFloats = nullptr;
	wxTextCtrl* roundInput = nullptr;

	wxCheckBox* manualUpdate = nullptr;
	wxButton* manualUpdateButton = nullptr;

	wxStaticText* threadsInputLabel = nullptr;
	wxTextCtrl* threadsInput = nullptr;

	wxGrid* addrList = nullptr;

	wxStaticText* resultsTxt = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;

	enum ids
	{
		MainWindowID,
		SelectProcessID,
		SelectValueTypeID,
		SelectScanTypeID,
		RoundFloatsID,
		FirstScanID,
		NextScanID,
		ScanSettingsID,
		ManualUpdateToggleID,
		ManualUpdateButtonID,
		AddressListID,
		OpenSavedListID,
		OpenBreakpointMenuID,
		UpdateTimerID
	};

	struct MemoryScanSettings
	{
		unsigned long long minAddress, maxAddress;

		int protection, scanType, roundingValue;

		bool roundFloats, scanImageMem, scanMappedMem, scanPrivateMem, alignMemory;
	};

	HANDLE procHandle = 0;

	std::vector<unsigned long long> addressPool;
	std::vector<unsigned char> bytes; // used to detect change in values

	const char* firstScans[7] =
	{
		"Equal", "Greater", "Less", "Not Equal", "Greater Or Equal", "Less Or Equal", "All"
	};

	const char* nextScans[12] =
	{
		"Equal", "Greater", "Less", "Not Equal", "Greater Or Equal", "Less Or Equal", "Increased By", "Decreased By", "Increased", "Decreased", "Changed", "Unchanged"
	};

	int base = 10;

	const int roundings[8] =
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
	int roundingPlace = 1;

	int threads = 1;

	int byteArraySize = 0;

	bool scanning = false;

	bool performedAllScan = false;

	void SelectProcess(wxCommandEvent& e);


	template <typename T> unsigned int FirstScan(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr);
	unsigned int FirstScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<unsigned long long>* addressesPtr);
	template <typename T> unsigned int FirstScanAll(MemoryScanSettings scanSettings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr);

	template <typename T> unsigned int NextScan(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr);
	unsigned int NextScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<unsigned long long>* addressesPtr);
	template <typename T> unsigned int NextScanAll(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr);

	MemoryScanSettings CreateScanSettingsStruct();

	bool ParseByteArray(wxString str, unsigned char** bytes);

	template <typename T> void UpdateList(bool isFloat);
	void UpdateListByteArray(int size);

	// gui functions

	void ResetScan();

	void FirstScanButtonPress(wxCommandEvent& e);
	void NextScanButtonPress(wxCommandEvent& e);

	void UpdateListOnTimer(wxTimerEvent& e);

	void WriteValueHandler(unsigned long long* address);

	void RightClickOptions(wxGridEvent& e);

	void RemoveRow(int row);

	void UpdateBaseAndRoundingAndThreads();

	void OpenScanSettings(wxCommandEvent& e);

	void OpenSavedMenu(wxCommandEvent& e);

	void OpenBreakpointMenu(wxCommandEvent& e);

	void UpdateRoundFloats(wxCommandEvent& e);

	void UpdateValueType(wxCommandEvent& e);

	void UpdateScanType(wxCommandEvent& e);

	void ToggleManualUpdate(wxCommandEvent& e);

	void ManuallyUpdate(wxCommandEvent& e);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};