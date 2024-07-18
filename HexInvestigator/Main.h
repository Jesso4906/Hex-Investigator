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
#include "HexCalculator.h"
#include "WriteMenu.h"
#include "DisassemblerMenu.h"

class SelectProcessMenu; // forward delcared becasue it needs to access the Main class

class Main : public wxFrame, public Utils
{
public:
	Main(); // constructor

	wxMenuBar* menuBar = nullptr;

	WriteMenu* writeMenu = nullptr;
	SavedMenu* savedMenu = nullptr;
	BreakpointMenu* breakpointMenu = nullptr;
	HexCalculator* hexCalculator = nullptr;
	DisassemblerMenu* disassembler = nullptr;

	wxButton* selectProc = nullptr;
	SelectProcessMenu* selectProcMenu = nullptr;

	wxChoice* selectValueType = nullptr;
	wxChoice* selectScanType = nullptr;
	wxButton* firstScan = nullptr;
	wxButton* nextScan = nullptr;

	wxButton* scanSettingsButton = nullptr;
	ScanSettingsMenu* scanSettingsMenu = nullptr;

	wxTextCtrl* valueInput = nullptr;
	wxTextCtrl* valueInput2 = nullptr; // used for the between scan type

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
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxTimer* updateTimer = nullptr;
	wxTimer* keyInputTimer = nullptr;

	HANDLE procHandle = 0;

	enum ids
	{
		MainWindowID,
		OpenWriteMenuID,
		OpenSavedListID,
		OpenBreakpointMenuID,
		OpenHexCalcID,
		OpenDisassemblerID,
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
		SelectToolID,
		UpdateTimerID,
		KeyInputTimerID
	};

	struct AddressModuleInfo 
	{
		uintptr_t rva;

		enum SectionType { Code, InitData, UninitData, Other } secitonType;

		wxString sectionName;

		wxString moduleName;
	};

	std::vector<uintptr_t> moduleHandles;
	std::vector<wxString> moduleNames;

	std::vector<uintptr_t> addressPool;
	std::vector<unsigned char> bytes; // used to detect change in values

	enum ScanType 
	{
		Equal, Greater, Less, NotEqual, GreaterOrEqual, LessOrEqual, Between, All,
		IncreasedBy, DecreasedBy, Increased, Decreased, Changed, Unchanged
	};

	const int numberOfFirstScanTypes = 8;
	const int numberOfNextScanTypes = 14;

	const char* scanTypeStrs[14] =
	{
		"Equal", "Greater", "Less", "Not Equal", "Greater Or Equal", "Less Or Equal", "Between", "All", 
		"Increased By", "Decreased By", "Increased", "Decreased", "Changed", "Unchanged"
	};

	struct MemoryScanSettings
	{
		uintptr_t minAddress, maxAddress;

		int protection;
		ScanType scanType;
		int roundingValue;

		bool roundFloats, scanImageMem, scanMappedMem, scanPrivateMem, alignMemory, scanByRegions;
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

	template <typename T> unsigned int FirstScan(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr);
	unsigned int FirstScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<uintptr_t>* addressesPtr);
	template <typename T> unsigned int FirstScanAll(MemoryScanSettings scanSettings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr);

	template <typename T> unsigned int NextScan(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr);
	unsigned int NextScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<uintptr_t>* addressesPtr);
	template <typename T> unsigned int NextScanAll(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr);

	MemoryScanSettings CreateScanSettingsStruct();

	bool ParseByteArray(wxString str, unsigned char** bytes);

	template <typename T> void UpdateList(bool isFloat);
	void UpdateListByteArray(int size, bool ascii);

	AddressModuleInfo GetAddressModuleInfo(uintptr_t address);

	void UpdateModuleHandles();

	void FreezeProcess(bool freeze);

	void OpenSelectProcessMenu(wxCommandEvent& e);
	void UpdateProcessSelection(HANDLE newProcHandle, wxString procName);

	void ResetScan();

	void FirstScanButtonPress(wxCommandEvent& e);
	void NextScanButtonPress(wxCommandEvent& e);

	void UpdateListOnTimer(wxTimerEvent& e);

	void CheckKeyInput(wxTimerEvent& e);

	void WriteValueHandler(wxString input, uintptr_t* address);

	void RightClickOptions(wxGridEvent& e);

	void RemoveRow(int row);

	void UpdateBaseAndRoundingAndThreads();

	void OpenScanSettings(wxCommandEvent& e);

	void UpdateRoundFloats(wxCommandEvent& e);

	void UpdateValueType(wxCommandEvent& e);

	void UpdateScanType(wxCommandEvent& e);

	void ToggleManualUpdate(wxCommandEvent& e);

	void ManuallyUpdate(wxCommandEvent& e);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};

class SelectProcessMenu : public wxFrame, public Utils
{
public:
	SelectProcessMenu(Main* mainPtr);

	Main* main = nullptr;

	wxTextCtrl* processNameInput = nullptr;

	wxCheckBox* useDebugPrivilege = nullptr;

	wxListBox* processList = nullptr;

	wxBoxSizer* vSizer = nullptr;

	wxArrayString* processNames = nullptr;

	std::vector<DWORD> originalProcIds;
	std::vector<DWORD> currentProcIds; // after search

	DWORD chosenProc = 0;

	enum ids
	{
		MainWindowID,
		ProcessNameInputID,
		UseDebugPrivilegeID,
		ProcessListID
	};

	void RefreshProcessList();

	void SearchProcessList(wxCommandEvent& e);

	void SelectProcess(wxCommandEvent& e);

	void UpdateDebugPrivilege(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};