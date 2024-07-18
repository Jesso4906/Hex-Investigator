#include "Main.h"

wxBEGIN_EVENT_TABLE(Main, wxFrame)
EVT_CLOSE(CloseApp)
EVT_BUTTON(SelectProcessID, OpenSelectProcessMenu)
EVT_CHOICE(SelectValueTypeID, UpdateValueType)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_CHOICE(SelectScanTypeID, UpdateScanType)
EVT_BUTTON(FirstScanID, FirstScanButtonPress)
EVT_BUTTON(NextScanID, NextScanButtonPress)
EVT_TIMER(UpdateTimerID, UpdateListOnTimer)
EVT_TIMER(KeyInputTimerID, CheckKeyInput)
EVT_BUTTON(ScanSettingsID, OpenScanSettings)
EVT_CHECKBOX(ManualUpdateToggleID, ToggleManualUpdate)
EVT_BUTTON(ManualUpdateButtonID, ManuallyUpdate)
EVT_CHECKBOX(RoundFloatsID, UpdateRoundFloats)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, MainWindowID, "Hex Investigator x64", wxPoint(50, 50), wxSize(550, 650))
{
	SetOwnBackgroundColour(backgroundColor);

	// procHandle must be updated in UpdateProcessSelection
	writeMenu = new WriteMenu(procHandle);
	savedMenu = new SavedMenu(procHandle, writeMenu);
	breakpointMenu = new BreakpointMenu(procHandle);
	hexCalculator = new HexCalculator();
	disassembler = new DisassemblerMenu(procHandle);

	menuBar = new wxMenuBar();

	wxMenu* toolMenu = new wxMenu();

	wxMenuItem* openWriteMenu = new wxMenuItem(0, OpenWriteMenuID, "Write Operation Setter");
	openWriteMenu->SetBackgroundColour(foregroundColor);
	openWriteMenu->SetTextColour(textColor);
	toolMenu->Append(openWriteMenu);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { writeMenu->OpenMenu(GetPosition()); }, OpenWriteMenuID);

	wxMenuItem* openSavedAddresses = toolMenu->Append(OpenSavedListID, "Saved Addresses");
	openSavedAddresses->SetBackgroundColour(foregroundColor);
	openSavedAddresses->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { savedMenu->OpenMenu(GetPosition()); }, OpenSavedListID);

	wxMenuItem* openBreakpointMenu = toolMenu->Append(OpenBreakpointMenuID, "Hardware Breakpoint Setter");
	openBreakpointMenu->SetBackgroundColour(foregroundColor);
	openBreakpointMenu->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { breakpointMenu->OpenMenu(GetPosition()); }, OpenBreakpointMenuID);

	wxMenuItem* openHexCalculator = toolMenu->Append(OpenHexCalcID, "Hex Calculator");
	openHexCalculator->SetBackgroundColour(foregroundColor);
	openHexCalculator->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { hexCalculator->OpenMenu(GetPosition()); }, OpenHexCalcID);

	wxMenuItem* openDisassembler = toolMenu->Append(OpenDisassemblerID, "Disassembler");
	openDisassembler->SetBackgroundColour(foregroundColor);
	openDisassembler->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { disassembler->OpenMenu(GetPosition()); }, OpenDisassemblerID);

	menuBar->Append(toolMenu, "Tools");
	this->SetMenuBar(menuBar);

	selectProc = new wxButton(this, SelectProcessID, "P", wxPoint(0, 0), wxSize(25, 25));
	selectProc->SetOwnBackgroundColour(foregroundColor);
	selectProc->SetOwnForegroundColour(textColor);

	selectProcMenu = new SelectProcessMenu(this);

	selectValueType = new wxChoice(this, SelectValueTypeID, wxPoint(0, 0), wxSize(70, 50), wxArrayString(numberOfValueTypes, typeStrs));
	selectValueType->SetSelection(1);
	selectValueType->SetOwnBackgroundColour(foregroundColor);
	selectValueType->SetOwnForegroundColour(textColor);

	selectScanType = new wxChoice(this, SelectScanTypeID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(numberOfFirstScanTypes, scanTypeStrs));
	selectScanType->SetSelection(Equal);
	selectScanType->SetOwnBackgroundColour(foregroundColor);
	selectScanType->SetOwnForegroundColour(textColor);

	baseInputLabel = new wxStaticText(this, wxID_ANY, "Base:");
	baseInputLabel->SetOwnForegroundColour(textColor);

	baseInput = new wxTextCtrl(this, wxID_ANY, "10", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	baseInput->SetOwnBackgroundColour(foregroundColor);
	baseInput->SetOwnForegroundColour(textColor);

	roundFloats = new wxCheckBox(this, RoundFloatsID, "Round to Place:");
	roundFloats->SetOwnForegroundColour(textColor);
	roundFloats->Disable();

	roundInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	roundInput->SetOwnBackgroundColour(foregroundColor);
	roundInput->SetOwnForegroundColour(textColor);
	roundInput->SetEditable(false);

	threadsInputLabel = new wxStaticText(this, wxID_ANY, "Threads:");
	threadsInputLabel->SetOwnForegroundColour(textColor);

	threadsInput = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(0, 0), wxSize(30, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	threadsInput->SetOwnBackgroundColour(foregroundColor);
	threadsInput->SetOwnForegroundColour(textColor);

	firstScan = new wxButton(this, FirstScanID, "First Scan", wxPoint(0, 0), wxSize(75, 25));
	firstScan->SetOwnBackgroundColour(foregroundColor);
	firstScan->SetOwnForegroundColour(textColor);
	firstScan->Disable();

	nextScan = new wxButton(this, NextScanID, "Next Scan", wxPoint(0, 0), wxSize(75, 25));
	nextScan->SetOwnBackgroundColour(foregroundColor);
	nextScan->SetOwnForegroundColour(textColor);
	nextScan->Disable();

	scanSettingsMenu = new ScanSettingsMenu(procHandle);

	scanSettingsButton = new wxButton(this, ScanSettingsID, "Scan Settings", wxPoint(0, 0), wxSize(100, 25));
	scanSettingsButton->SetOwnBackgroundColour(foregroundColor);
	scanSettingsButton->SetOwnForegroundColour(textColor);
	scanSettingsButton->Disable();

	valueInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(9999, 25));
	valueInput->SetOwnBackgroundColour(foregroundColor);
	valueInput->SetOwnForegroundColour(textColor);

	valueInput2 = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(0, 0), wxSize(9999, 25));
	valueInput2->SetOwnBackgroundColour(foregroundColor);
	valueInput2->SetOwnForegroundColour(textColor);
	valueInput2->Hide();

	manualUpdate = new wxCheckBox(this, ManualUpdateToggleID, "Manually Update List");
	manualUpdate->SetOwnForegroundColour(textColor);

	manualUpdateButton = new wxButton(this, ManualUpdateButtonID, "Update", wxPoint(0, 0), wxSize(75, 25));
	manualUpdateButton->SetOwnBackgroundColour(foregroundColor);
	manualUpdateButton->SetOwnForegroundColour(textColor);
	manualUpdateButton->Disable();

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(backgroundColor);
	addrList->SetLabelTextColour(textColor);
	addrList->SetDefaultCellBackgroundColour(foregroundColor);
	addrList->SetDefaultCellTextColour(textColor);

	addrList->CreateGrid(0, 3);
	addrList->EnableGridLines(false);
	addrList->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Offset");
	addrList->SetColLabelValue(2, "Value");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 200);
	addrList->SetColSize(1, 80);
	addrList->SetColSize(2, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	resultsTxt = new wxStaticText(this, wxID_ANY, "Results:");
	resultsTxt->SetOwnForegroundColour(textColor);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(selectProc, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	row1Sizer->Add(selectValueType, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(selectScanType, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(firstScan, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(nextScan, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(scanSettingsButton, 0, wxTOP | wxRIGHT, 10);

	row2Sizer->Add(baseInputLabel, 0, wxRIGHT | wxLEFT, 10);
	row2Sizer->Add(baseInput, 0, wxRIGHT, 15);
	row2Sizer->Add(roundFloats, 0);
	row2Sizer->Add(roundInput, 0, wxRIGHT, 15);
	row2Sizer->Add(manualUpdate, 0, wxRIGHT, 5);
	row2Sizer->Add(manualUpdateButton, 0, wxRIGHT, 10);
	row2Sizer->Add(threadsInputLabel, 0, wxRIGHT, 5);
	row2Sizer->Add(threadsInput, 0, wxRIGHT, 10);

	row3Sizer->Add(valueInput, 0, wxALL, 10);
	row3Sizer->Add(valueInput2, 0, wxTOP | wxRIGHT | wxBOTTOM, 10);

	row4Sizer->Add(resultsTxt, 0, wxLEFT, 10);
	
	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(row4Sizer, 0, wxEXPAND);
	vSizer->Add(addrList, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	SetSizer(vSizer);

	updateTimer = new wxTimer(this, UpdateTimerID);

	keyInputTimer = new wxTimer(this, KeyInputTimerID);
	keyInputTimer->Start(100);
}

// memory scan functions

template <typename T> unsigned int Main::FirstScan(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<uintptr_t> newAddresses;
	std::vector<unsigned char> newBytes;
	
	uintptr_t baseAddress = scanSettings.minAddress;

	int size = sizeof(T);

	int offset = 1;
	if (scanSettings.alignMemory) { offset = size; }

	// the use of + converts it to a function ptr so the variable meetsCondition can be reassigned
	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal
	switch (scanSettings.scanType)
	{
	case Greater:
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case Less:
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case NotEqual:
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case GreaterOrEqual:
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case LessOrEqual:
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (uintptr_t*)baseAddress, &mbi, sizeof(mbi)))
	{
		size_t regionSize = mbi.RegionSize - (baseAddress - (uintptr_t)mbi.BaseAddress); // VirtualQueryEx rounds down to the nearest page, so baseAddress may not always equal mbi.BaseAddress
		
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			unsigned char* buffer = new unsigned char[mbi.RegionSize];

			ReadProcessMemory(procHandle, (void*)baseAddress, buffer, regionSize, 0);

			for (int i = 0; i < regionSize; i += offset)
			{
				T value = *(T*)(buffer + i);

				if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

				bool addAddress = false;
				if (scanSettings.scanType == Between) 
				{
					if (value >= targetValue && value <= targetValue2) 
					{
						addAddress = true;
					}
				}
				else if (meetsCondition(value, targetValue))
				{
					addAddress = true;
				}

				if (addAddress) 
				{
					newAddresses.push_back(baseAddress + i);

					unsigned char* valueBytes = (unsigned char*)&value;
					newBytes.insert(newBytes.end(), valueBytes, valueBytes + size);

					results++;
				}
			}

			delete[] buffer;
		}

		baseAddress += regionSize;
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

unsigned int Main::FirstScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<uintptr_t>* addressesPtr)
{
	std::vector<uintptr_t> newAddresses;
	
	uintptr_t baseAddress = scanSettings.minAddress;

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (uintptr_t*)baseAddress, &mbi, sizeof(mbi)))
	{
		size_t regionSize = mbi.RegionSize - (baseAddress - (uintptr_t)mbi.BaseAddress); // VirtualQueryEx rounds down to the nearest page, so baseAddress may not always equal mbi.BaseAddress
		
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			unsigned char* buffer = new unsigned char[regionSize];

			ReadProcessMemory(procHandle, (void*)baseAddress, buffer, regionSize, 0);

			for (int i = 0; i < regionSize; i++)
			{
				for (int j = 0; j < size; j++)
				{
					if (*(buffer + i + j) != targetBytes[j]) { break; }
					else if (j == (size - 1))
					{
						newAddresses.push_back(baseAddress + i);
						results++;
					}
				}
			}

			delete[] buffer;
		}

		baseAddress += regionSize;
	}

	*addressesPtr = newAddresses;

	return results;
}

template <typename T> unsigned int Main::FirstScanAll(MemoryScanSettings scanSettings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<uintptr_t> newAddresses;
	std::vector<unsigned char> newBytes;
	
	uintptr_t baseAddress = scanSettings.minAddress;

	int offset = 1;
	if (scanSettings.alignMemory) { offset = sizeof(T); }

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (uintptr_t*)baseAddress, &mbi, sizeof(mbi)))
	{
		size_t regionSize = mbi.RegionSize - (baseAddress - (uintptr_t)mbi.BaseAddress); // VirtualQueryEx rounds down to the nearest page, so baseAddress may not always equal mbi.BaseAddress
		
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			newAddresses.push_back(baseAddress); // store the base address
			newAddresses.push_back(regionSize); // store the size
			
			unsigned char* buffer = new unsigned char[regionSize];
			ReadProcessMemory(procHandle, (void*)baseAddress, buffer, regionSize, 0);
			newBytes.insert(newBytes.end(), buffer, buffer + regionSize); // store data

			delete[] buffer;

			results += regionSize / offset;
		}

		baseAddress += regionSize;
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

template <typename T> unsigned int Main::NextScan(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<uintptr_t> newAddresses;
	std::vector<unsigned char> newBytes;

	int size = sizeof(T);

	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal, Increased By, Decreased By, Unchanged
	switch (scanSettings.scanType)
	{
	case Greater: 
	case Increased:
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case Less:
	case Decreased:
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case NotEqual:
	case Changed:
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case GreaterOrEqual:
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case LessOrEqual:
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned char* dataPtr = bytes.data();

	int results = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i++)
	{
		T value;
		ReadProcessMemory(procHandle, (uintptr_t*)addressPool[i], &value, size, 0);

		if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

		T otherValue = targetValue;

		if (scanSettings.scanType > All) // scan types that need the old value
		{
			otherValue = *(T*)(dataPtr + (i * size)); // set to last value

			if (scanSettings.scanType == IncreasedBy) { otherValue += targetValue; } // checking increase by targetValue
			else if (scanSettings.scanType == DecreasedBy) { otherValue -= targetValue; } // checking decrease by targetValue
		}

		bool addAddress = false;
		if (scanSettings.scanType == Between) 
		{
			if (value >= targetValue && value <= targetValue2) 
			{
				addAddress = true;
			}
		}
		else if (meetsCondition(value, otherValue))
		{
			addAddress = true;
		}

		if (addAddress) 
		{
			newAddresses.push_back(addressPool[i]);

			unsigned char* valueBytes = (unsigned char*)&value;
			newBytes.insert(newBytes.end(), valueBytes, valueBytes + size);

			results++;
		}
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

unsigned int Main::NextScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<uintptr_t>* addressesPtr)
{
	std::vector<uintptr_t> newAddresses;

	unsigned int results = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i++)
	{
		unsigned char* value = new unsigned char[size];
		ReadProcessMemory(procHandle, (uintptr_t*)addressPool[i], value, size, 0);

		for (int j = 0; j < size; j++)
		{
			if (value[j] != targetBytes[j]) { break; }
			else if (j == (size - 1))
			{
				newAddresses.push_back(addressPool[i]);
				results++;
			}
		}

		delete[] value;
	}

	*addressesPtr = newAddresses;

	return results;
}

template <typename T> unsigned int Main::NextScanAll(MemoryScanSettings scanSettings, T targetValue, T targetValue2, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) // this function is called after FirstScanAll because of the different way the addresses are stored
{
	std::vector<uintptr_t> newAddresses;
	std::vector<unsigned char> newBytes;

	int size = sizeof(T);

	int offset = 1;
	if (scanSettings.alignMemory) { offset = size; }

	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal, Increased By, Decreased By, Unchanged
	switch (scanSettings.scanType)
	{
	case Greater: 
	case Increased:
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case Less:
	case Decreased:
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case NotEqual:
	case Changed:
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case GreaterOrEqual:
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case LessOrEqual:
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned char* dataPtr = bytes.data();

	unsigned int results = 0;

	int byteIndex = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i += 2) // after FirstScanAll, region bases and the size are stored in pairs
	{
		uintptr_t address = addressPool[i];
		int regionSize = addressPool[i + 1];
		for (int j = 0; j < regionSize; j += offset)
		{
			T value;
			ReadProcessMemory(procHandle, (uintptr_t*)(address + j), &value, size, 0);

			if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

			T otherValue = targetValue;

			if (scanSettings.scanType > All) // scan types that need the old value
			{ 
				otherValue = *(T*)(dataPtr + byteIndex); // set to last value

				if (scanSettings.scanType == IncreasedBy) { otherValue += targetValue; } // checking increase by targetValue
				else if (scanSettings.scanType == DecreasedBy) { otherValue -= targetValue; } // checking decrease by targetValue
			}

			bool addAddress = false;
			if (scanSettings.scanType == Between) 
			{
				if (value >= targetValue && value <= targetValue2) 
				{
					addAddress = true;
				}
			}
			else if (meetsCondition(value, otherValue))
			{
				addAddress = true;
			}

			if (addAddress) 
			{
				newAddresses.push_back(address + j);

				unsigned char* valueBytes = (unsigned char*)&value;
				newBytes.insert(newBytes.end(), valueBytes, valueBytes + size);

				results++;
			}

			byteIndex += offset;
		}
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

Main::MemoryScanSettings Main::CreateScanSettingsStruct()
{
	MemoryScanSettings memoryScanSettings =
	{
		scanSettingsMenu->minAddress,
		scanSettingsMenu->maxAddress,
		scanSettingsMenu->currentProtection,
		(ScanType)selectScanType->GetSelection(),
		roundings[roundingPlace],
		roundFloats->IsChecked(),
		scanSettingsMenu->image,
		scanSettingsMenu->mapped,
		scanSettingsMenu->priv,
		scanSettingsMenu->alignMemory
	};

	return memoryScanSettings;
}

template <typename T> void Main::UpdateList(bool isFloat)
{
	bool round = roundFloats->IsChecked();

	UpdateBaseAndRoundingAndThreads();

	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }

		T value;
		ReadProcessMemory(procHandle, (uintptr_t*)addressPool[i], &value, sizeof(T), 0);

		// convert the value to the appropriate string
		std::string valueStr;
		if (round)
		{
			std::stringstream roundValue;
			roundValue << std::fixed << std::setprecision(roundingPlace) << value;
			valueStr = roundValue.str();
		}
		else if (isFloat)
		{
			valueStr = std::to_string(value);
		}
		else
		{
			char buffer[100];
			if (value < 0) { valueStr = std::string(lltoa(value, buffer, base)); }
			else { valueStr = std::string(ulltoa(value, buffer, base)); }
		}

		addrList->SetCellValue(i, 2, valueStr);

		if (i != 0)
		{
			std::stringstream offsetToHex;
			offsetToHex << std::hex << (addressPool[i] - addressPool[i - 1]);
			addrList->SetCellValue(i, 1, offsetToHex.str());
		}

		AddressModuleInfo info = {};
		if (scanSettingsMenu->displayModuleInfo->IsChecked()) { info = GetAddressModuleInfo(addressPool[i]); }

		std::stringstream addressToHex;

		char firstChar = 0;
		if (!addrList->GetCellValue(i, 0).IsEmpty()) { firstChar = addrList->GetCellValue(i, 0).GetChar(0); }

		if (info.rva != 0 && firstChar != '<')
		{
			addressToHex << std::hex << info.rva;

			addrList->SetCellValue(i, 0, "<" + info.moduleName + ">+" + addressToHex.str() + " " + info.sectionName);

			switch (info.secitonType)
			{
			case info.SectionType::Code:
				addrList->SetCellTextColour(i, 0, wxColour(255, 30, 30));
				break;
			case info.SectionType::InitData:
				addrList->SetCellTextColour(i, 0, wxColour(30, 255, 30));
				break;
			case info.SectionType::UninitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 150, 255));
				break;
			case info.SectionType::Other:
				addrList->SetCellTextColour(i, 0, wxColour(220, 120, 255));
				break;
			}
		}
		else if (info.rva == 0 && (addrList->GetCellValue(i, 0) == "" || firstChar == '<'))
		{
			addressToHex << std::hex << addressPool[i];

			addrList->SetCellValue(i, 0, addressToHex.str());
			addrList->SetCellTextColour(i, 0, textColor);
		}
	}
}

void Main::UpdateListByteArray(int size, bool ascii)
{
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }

		unsigned char* value = new unsigned char[size];
		ReadProcessMemory(procHandle, (uintptr_t*)addressPool[i], value, size, 0);

		if (ascii) 
		{
			addrList->SetCellValue(i, 2, std::string((const char*)value).substr(0, size));
		}
		else 
		{
			std::string valueStr;
			for (int i = 0; i < size; i++)
			{
				std::stringstream byteToHex;
				byteToHex << std::hex << (int)value[i];
				valueStr += byteToHex.str() + ' ';
			}

			delete[] value;

			addrList->SetCellValue(i, 2, valueStr);
		}

		if (i != 0)
		{
			std::stringstream offsetToHex;
			offsetToHex << std::hex << (addressPool[i] - addressPool[i - 1]);
			addrList->SetCellValue(i, 1, offsetToHex.str());
		}

		AddressModuleInfo info = {};
		if (scanSettingsMenu->displayModuleInfo->IsChecked()) { info = GetAddressModuleInfo(addressPool[i]); }

		std::stringstream addressToHex;

		char firstChar = 0;
		if (!addrList->GetCellValue(i, 0).IsEmpty()) { firstChar = addrList->GetCellValue(i, 0).GetChar(0); }

		if (info.rva != 0 && firstChar != '<')
		{
			addressToHex << std::hex << info.rva;

			addrList->SetCellValue(i, 0, "<" + info.moduleName + ">+" + addressToHex.str() + " " + info.sectionName);

			switch (info.secitonType)
			{
			case info.SectionType::Code:
				addrList->SetCellTextColour(i, 0, wxColour(255, 30, 30));
				break;
			case info.SectionType::InitData:
				addrList->SetCellTextColour(i, 0, wxColour(30, 255, 30));
				break;
			case info.SectionType::UninitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 150, 255));
				break;
			case info.SectionType::Other:
				addrList->SetCellTextColour(i, 0, wxColour(220, 120, 255));
				break;
			}
		}
		else if (info.rva == 0 && (addrList->GetCellValue(i, 0) == "" || firstChar == '<'))
		{
			addressToHex << std::hex << addressPool[i];

			addrList->SetCellValue(i, 0, addressToHex.str());
			addrList->SetCellTextColour(i, 0, textColor);
		}
	}
}

bool Main::ParseByteArray(wxString str, unsigned char** bytes)
{
	int len = str.Length();
	if (len < 2 || len % 2 != 0) { return false; }

	*bytes = new unsigned char[len / 2];

	int currentByte = 0;
	for (int i = 0; i < len; i += 2)
	{
		unsigned int byte = 0;
		if (!str.SubString(i, i + 1).ToUInt(&byte, 16))
		{
			delete[] * bytes;
			return false;
		}
		(*bytes)[currentByte] = (unsigned char)byte;
		currentByte++;
	}

	byteArraySize = len / 2;

	return true;
}

Main::AddressModuleInfo Main::GetAddressModuleInfo(uintptr_t address)
{
	AddressModuleInfo info = {};
	
	for (int i = 0; i < moduleHandles.size(); i++)
	{
		if (address < moduleHandles[i]) { continue; }

		uintptr_t relativeAddress = address - moduleHandles[i];

		//https://reverseengineering.stackexchange.com/questions/6077/get-sections-names-and-headers-for-a-file-using-c

		IMAGE_DOS_HEADER dosHeader = {};
		ReadProcessMemory(procHandle, (uintptr_t*)moduleHandles[i], &dosHeader, sizeof(dosHeader), nullptr);

		uintptr_t imageNtHeadersAddress = (moduleHandles[i] + (uintptr_t)dosHeader.e_lfanew);
		
		IMAGE_NT_HEADERS imageNtHeader = {};
		ReadProcessMemory(procHandle, (uintptr_t*)imageNtHeadersAddress, &imageNtHeader, sizeof(imageNtHeader), nullptr);

		for (int j = 0; j < imageNtHeader.FileHeader.NumberOfSections; j++)
		{
			IMAGE_SECTION_HEADER section = {};

			uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * j) + imageNtHeadersAddress + sizeof(imageNtHeader.Signature) + sizeof(imageNtHeader.FileHeader) + imageNtHeader.FileHeader.SizeOfOptionalHeader;
			
			ReadProcessMemory(procHandle, (uintptr_t*)sectionAddress, &section, sizeof(section), nullptr);
			
			DWORD sectionBase = section.VirtualAddress;
			DWORD sectionEnd = sectionBase + section.Misc.VirtualSize;
			
			if (relativeAddress >= sectionBase && relativeAddress <= sectionEnd)
			{
				info.rva = relativeAddress;

				if (section.Characteristics & IMAGE_SCN_CNT_CODE) { info.secitonType = info.SectionType::Code; }
				else if (section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) { info.secitonType = info.SectionType::InitData; }
				else if (section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) { info.secitonType = info.SectionType::UninitData; }
				else { info.secitonType = info.SectionType::Other; }

				info.sectionName = wxString(section.Name);
				info.moduleName = moduleNames[i];
				
				return info;
			}
		}
	}

	return info;
}

void Main::UpdateModuleHandles() 
{
	moduleHandles.clear();
	moduleHandles.shrink_to_fit();
	moduleNames.clear();
	moduleNames.shrink_to_fit();
	
	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(procHandle));

	if (modSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(modSnap, &modEntry))
		{
			do
			{
				moduleHandles.push_back((uintptr_t)modEntry.hModule);
				moduleNames.push_back(wxString(modEntry.szModule));

			} while (Module32Next(modSnap, &modEntry));
		}
	}
	CloseHandle(modSnap);
}

void Main::FreezeProcess(bool freeze) 
{
	DWORD procId = GetProcessId(procHandle);
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, procId);

	if (snap != INVALID_HANDLE_VALUE) 
	{
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(snap, &threadEntry)) 
		{
			do
			{
				if (threadEntry.th32OwnerProcessID == procId)
				{
					HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

					if (freeze) { SuspendThread(threadHandle); }
					else { ResumeThread(threadHandle); }

					CloseHandle(threadHandle);
				}
			} while (Thread32Next(snap, &threadEntry));
		}

		CloseHandle(snap);
	}
}

// other functions

void Main::CheckKeyInput(wxTimerEvent& e)
{
	if(scanSettingsMenu->scanKeybind == -1) { return; }
	
	wxCommandEvent empty = 0;
	
	if (GetAsyncKeyState(scanSettingsMenu->scanKeybind) & 1)
	{
		if (scanning) { NextScanButtonPress(empty); }
		else { FirstScanButtonPress(empty); }
	}
}

void Main::OpenSelectProcessMenu(wxCommandEvent& e)
{
	selectProcMenu->OpenMenu(GetPosition());
}

void Main::UpdateProcessSelection(HANDLE newProcHandle, wxString procName) 
{
	ResetScan();

	procHandle = newProcHandle;

	savedMenu->procHandle = procHandle;
	breakpointMenu->procHandle = procHandle;
	scanSettingsMenu->procHandle = procHandle;
	writeMenu->procHandle = procHandle;
	disassembler->procHandle = procHandle;

	firstScan->Enable();
	scanSettingsButton->Enable();

	this->SetTitle("Hex Investigator x64 - [attached to " + procName + "]");
}

void Main::FirstScanButtonPress(wxCommandEvent& e)
{
	if (scanning)
	{
		// Reset if first scan button is pressed while already scanning
		
		int answer = wxMessageBox("Are you sure you want to reset the scan?", "Reset Scan", wxYES_NO, this);
		if (answer == wxNO) { return; }

		ResetScan();
		return;
	}

	UpdateModuleHandles();

	UpdateBaseAndRoundingAndThreads();

	MemoryScanSettings memoryScanSettings = CreateScanSettingsStruct();
	uintptr_t originalMax = memoryScanSettings.maxAddress;
	
	performedAllScan = selectScanType->GetSelection() == All;

	unsigned int results = 0;

	ValueType valueType = (ValueType)selectValueType->GetSelection();

	std::vector<std::thread> scanThreads(threads);

	std::vector<std::vector<uintptr_t>> addressLists(threads);
	std::vector<std::vector<unsigned char>> byteLists(threads);

	for (int i = 0; i < threads; i++) 
	{
		memoryScanSettings.maxAddress = originalMax * ((i+1) / (float)threads);

		switch (valueType)
		{
		case Int64:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<long long>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			long long targetValue = 0;
			if (!valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			long long targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToLongLong(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, long long targetValue, long long targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, targetValue, targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int32:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<int>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, int targetValue, int targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, targetValue, targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int16:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<short>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, short targetValue, short targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, (short)targetValue, (short)targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int8:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<char>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, char targetValue, char targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, (char)targetValue, (char)targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt64:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned long long>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned long long targetValue = 0;
			if (!valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned long long targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToULongLong(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, unsigned long long targetValue, unsigned long long targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, targetValue, targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt32:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned int>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, unsigned int targetValue, unsigned int targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, targetValue, targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt16:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned short>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, unsigned short targetValue, unsigned short targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, (unsigned short)targetValue, (unsigned short)targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt8:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned char>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, unsigned char targetValue, unsigned char targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, (unsigned char)targetValue, (unsigned char)targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Float:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<float>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			double targetValue = 0;
			if (!valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			double targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToDouble(&targetValue2)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, float targetValue, float targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<float>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, (float)targetValue, (float)targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Double:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<double>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			double targetValue = 0;
			if (!valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			double targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!valueInput2->GetValue().ToDouble(&targetValue2)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			scanThreads[i] = std::thread([](Main* main, double targetValue, double targetValue2, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<double>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, targetValue, targetValue2, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case Bytes:
		{
			unsigned char* targetValue;
			if (!ParseByteArray(valueInput->GetValue(), &targetValue)) { wxMessageBox("Invalid Value. Should be in format FB23EA...", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr) -> void
				{ *results += main->FirstScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		case ASCII:
		{
			wxString val = valueInput->GetValue();
			byteArraySize = val.length();
			const char* targetValue = new char[byteArraySize];
			memcpy((void*)targetValue, val.ToAscii().data(), byteArraySize);

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr) -> void
				{ *results += main->FirstScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, (unsigned char*)targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		}

		memoryScanSettings.minAddress = memoryScanSettings.maxAddress;
	}

	bool freezeDuringScan = scanSettingsMenu->freezeProcess->IsChecked();

	if (freezeDuringScan) { FreezeProcess(true); }

	for (int i = 0; i < threads; i++) 
	{
		scanThreads[i].join(); // wait for thread to finish

		addressPool.insert(addressPool.end(), addressLists[i].begin(), addressLists[i].end());
		addressLists[i].clear(); addressLists[i].shrink_to_fit();

		bytes.insert(bytes.end(), byteLists[i].begin(), byteLists[i].end());
		byteLists[i].clear(); byteLists[i].shrink_to_fit();
	}

	// add commmas
	std::string resultsStr = std::to_string(results);
	int n = resultsStr.length() - 3;
	while (n > 0) 
	{
		resultsStr.insert(n, ",");
		n -= 3;
	}

	if (results > 10000)
	{
		resultsTxt->SetLabel("Results: " + resultsStr + " (10,000 shown)");
		results = 10000;
	}
	else
	{
		resultsTxt->SetLabel("Results: " + resultsStr);
	}

	if (results != 0 && !manualUpdate->IsChecked()) 
	{ 
		addrList->AppendRows(results);
	}

	if (freezeDuringScan) { FreezeProcess(false); }
	
	nextScan->Enable();
	selectValueType->Disable();
	scanSettingsMenu->Hide();

	baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
	baseInputLabel->Refresh();
	baseInput->SetEditable(false);
	baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
	baseInput->Refresh();

	firstScan->SetLabel("Reset Scan");

	if (!manualUpdate->IsChecked()) { updateTimer->Start(updateRate); }

	scanning = true;

	if (valueType > Double) { return; } // dont change the scan options if the type is a byte array
	selectScanType->Set(wxArrayString(numberOfNextScanTypes, scanTypeStrs));
	selectScanType->SetSelection(Equal);
	valueInput->Show();
}

void Main::NextScanButtonPress(wxCommandEvent& e)
{
	if (manualUpdate->IsChecked()) { updateTimer->Stop(); }
	UpdateBaseAndRoundingAndThreads();

	bool noInput = selectScanType->GetSelection() > DecreasedBy; // input will not be read if scan type is Increased, Decreased, Changed, or Unchanged

	MemoryScanSettings memoryScanSettings = CreateScanSettingsStruct();
	uintptr_t originalMax = addressPool.size();

	unsigned int results = 0;

	std::vector<std::thread> scanThreads(threads);

	std::vector<std::vector<uintptr_t>> addressLists(threads);
	std::vector<std::vector<unsigned char>> byteLists(threads);

	ValueType valueType = (ValueType)selectValueType->GetSelection();

	for (int i = 0; i < threads; i++) 
	{
		memoryScanSettings.minAddress = originalMax * (i / (float)threads);
		memoryScanSettings.maxAddress = originalMax * ((i+1) / (float)threads);
		if (performedAllScan && memoryScanSettings.minAddress % 2 != 0) { memoryScanSettings.minAddress--; } // the addresses are stored at even indexes while the region size is at the odd index
		if (performedAllScan && memoryScanSettings.maxAddress % 2 != 0) { memoryScanSettings.maxAddress--; }
		
		switch (valueType)
		{
		case Int64:
		{
			long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			long long targetValue2 = 0;
			if (memoryScanSettings.scanType == Between)
			{
				if (!noInput && !valueInput2->GetValue().ToLongLong(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan) 
			{ 
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, long long targetValue, long long targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, long long targetValue, long long targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int32:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between)
			{
				if (!noInput && !valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, int targetValue, int targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, int targetValue, int targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int16:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between)
			{
				if (!noInput && !valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, short targetValue, short targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (short)targetValue, (short)targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, short targetValue, short targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (short)targetValue, (short)targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Int8:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between)
			{
				if (!noInput && !valueInput2->GetValue().ToInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, char targetValue, char targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (char)targetValue, (char)targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, char targetValue, char targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (char)targetValue, (char)targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt64:
		{
			unsigned long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned long long targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToULongLong(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned long long targetValue, unsigned long long targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned long long targetValue, unsigned long long targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned long long>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt32:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned int targetValue, unsigned int targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned int targetValue, unsigned int targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned int>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt16:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			
			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned short targetValue, unsigned short targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (unsigned short)targetValue, (unsigned short)targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned short targetValue, unsigned short targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned short>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (unsigned short)targetValue, (unsigned short)targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case UInt8:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			unsigned int targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToUInt(&targetValue2, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned char targetValue, unsigned char targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (unsigned char)targetValue, (unsigned char)targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned char targetValue, unsigned char targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned char>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (unsigned char)targetValue, (unsigned char)targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Float:
		{
			double targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			double targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToDouble(&targetValue2)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, float targetValue, float targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<float>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (float)targetValue, (float)targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, float targetValue, float targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<float>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (float)targetValue, (float)targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Double:
		{
			double targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			double targetValue2 = 0;
			if (memoryScanSettings.scanType == Between) 
			{
				if (!noInput && !valueInput2->GetValue().ToDouble(&targetValue2)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			}

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, double targetValue, double targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<double>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, double targetValue, double targetValue2, unsigned int* results, std::vector<uintptr_t>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<double>(settings, targetValue, targetValue2, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, targetValue2, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case Bytes:
		{
			unsigned char* targetValue;
			if (!ParseByteArray(valueInput->GetValue(), &targetValue)) { wxMessageBox("Invalid Value. Should be in format FB23EA...", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr) -> void
				{ *results += main->NextScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		case ASCII:
		{
			wxString val = valueInput->GetValue();
			byteArraySize = val.length();
			const char* targetValue = new char[byteArraySize];
			memcpy((void*)targetValue, val.ToAscii().data(), byteArraySize);

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<uintptr_t>* addressesPtr) -> void
				{ *results += main->NextScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, (unsigned char*)targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		}
	}

	bool freezeDuringScan = scanSettingsMenu->freezeProcess->IsChecked();

	if (freezeDuringScan) { FreezeProcess(true); }

	for (int i = 0; i < threads; i++)
	{
		scanThreads[i].join(); // wait for thread to finish
	}

	addressPool.clear();
	bytes.clear();
	for (int i = 0; i < threads; i++) 
	{
		addressPool.insert(addressPool.end(), addressLists[i].begin(), addressLists[i].end());

		bytes.insert(bytes.end(), byteLists[i].begin(), byteLists[i].end());
	}
	addressPool.shrink_to_fit();
	bytes.shrink_to_fit();

	int rows = addrList->GetNumberRows();
	if (rows > 0) { addrList->DeleteRows(0, rows); }

	// add commmas
	std::string resultsStr = std::to_string(results);
	int n = resultsStr.length() - 3;
	while (n > 0)
	{
		resultsStr.insert(n, ",");
		n -= 3;
	}
	
	if (results > 10000)
	{
		resultsTxt->SetLabel("Results: " + resultsStr + " (10,000 shown)");
		results = 10000;
	}
	else
	{
		resultsTxt->SetLabel("Results: " + resultsStr);
	}

	if (results != 0 && !manualUpdate->IsChecked()) 
	{ 
		addrList->AppendRows(results);
	}

	if (freezeDuringScan) { FreezeProcess(false); }

	performedAllScan = false;

	manualUpdate->Enable();  // re enable after all scan

	if (manualUpdate->IsChecked()) { manualUpdateButton->Enable(); }
}

void Main::UpdateListOnTimer(wxTimerEvent& e) 
{
	ValueType type = (ValueType)selectValueType->GetSelection();
	switch (type)
	{
	case Int64:
		UpdateList<long long>(false);
		break;
	case Int32:
		UpdateList<int>(false);
		break;
	case Int16:
		UpdateList<short>(false);
		break;
	case Int8:
		UpdateList<char>(false);
		break;
	case UInt64:
		UpdateList<unsigned long long>(false);
		break;
	case UInt32:
		UpdateList<unsigned int>(false);
		break;
	case UInt16:
		UpdateList<unsigned short>(false);
		break;
	case UInt8:
		UpdateList<unsigned char>(false);
		break;
	case Float:
		UpdateList<float>(true);
		break;
	case Double:
		UpdateList<double>(true);
		break;
	case Bytes:
		UpdateListByteArray(byteArraySize, false);
		break;
	case ASCII:
		UpdateListByteArray(byteArraySize, true);
		break;
	}
}

void Main::WriteValueHandler(wxString input, uintptr_t* address)
{
	if (input == "") { return; }

	//Get and check base
	baseInput->GetValue().ToInt(&base);
	if (base < 2 || base > 36) { base = 10; baseInput->SetValue("10"); }
	
	ValueType type = (ValueType)selectValueType->GetSelection();
	switch (type)
	{
		case Int64:
		{
			long long targetValue = 0;
			if (!input.ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case Int32:
		{
			int targetValue = 0;
			if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case Int16:
		{
			int temp = 0;
			if (!input.ToInt(&temp, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			short targetValue = (short)temp;
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case Int8:
		{
			int temp = 0;
			if (!input.ToInt(&temp, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			char targetValue = (char)temp;
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case UInt64:
		{
			unsigned long long targetValue = 0;
			if (!input.ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case UInt32:
		{
			unsigned int targetValue = 0;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case UInt16:
		{
			unsigned int temp = 0;
			if (!input.ToUInt(&temp, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			unsigned short targetValue = (unsigned short)temp;
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case UInt8:
		{
			unsigned int temp = 0;
			if (!input.ToUInt(&temp, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			unsigned char targetValue = (unsigned char)temp;
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case Float:
		{
			double doubleVal = 0;
			if (!input.ToDouble(&doubleVal)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			float targetValue = (float)doubleVal;
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
		case Double:
		{
			double targetValue = 0;
			if (!input.ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteProcessMemory(procHandle, address, &targetValue, sizeof(targetValue), nullptr);
			break;
		}
	}
}

void Main::ResetScan()
{
	nextScan->Disable();
	selectValueType->Enable();
	valueInput->Enable();

	selectScanType->Set(wxArrayString(numberOfFirstScanTypes, scanTypeStrs));
	selectScanType->SetSelection(Equal);

	wxCommandEvent e;
	UpdateValueType(e);
	UpdateScanType(e);

	updateTimer->Stop();

	addressPool.clear();
	addressPool.shrink_to_fit();
	bytes.clear();
	bytes.shrink_to_fit();

	if (addrList->GetNumberRows() > 0) { addrList->DeleteRows(0, addrList->GetNumberRows()); }

	resultsTxt->SetLabel("Results: 0");

	firstScan->SetLabel("First Scan");

	scanning = false;
}

void Main::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on
	wxArrayInt selectedRows = addrList->GetSelectedRows(); // all rows also selected

	wxMenuItem* save = new wxMenuItem(0, 100, "Save");
	save->SetBackgroundColour(foregroundColor);
	save->SetTextColour(textColor);
	menu.Append(save);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (!savedMenu->IsVisible()) { savedMenu->OpenMenu(GetPosition()); }

			SavedMenu::SavedEntry savedEntry;
			savedEntry.address = addressPool[row];
			savedEntry.type = (ValueType)selectValueType->GetSelection();
			savedEntry.base = base;
			savedEntry.byteArraySize = byteArraySize;

			if (selectedRows.IsEmpty()) { savedMenu->AddAddress(savedEntry); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				savedEntry.address = addressPool[selectedRows.Item(i)];
				savedMenu->AddAddress(savedEntry);
			}

		}, 100);

	if (selectValueType->GetSelection() < Bytes) // cant write if its a byte array
	{
		wxMenuItem* write = menu.Append(101, "Write Value");
		write->SetBackgroundColour(foregroundColor);
		write->SetTextColour(textColor);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
			{ 
				wxString input = wxGetTextFromUser("New Value (using current base):", "Write Value");
				
				if (selectedRows.IsEmpty()) { WriteValueHandler(input, (uintptr_t*)addressPool[row]); }

				for (int i = 0; i < selectedRows.GetCount(); i++)
				{
					WriteValueHandler(input, (uintptr_t*)addressPool[selectedRows.Item(i)]);
				}
			}, 101);

		wxMenuItem* setWriteOp = new wxMenuItem(0, 106, "Set Write Operation");
		setWriteOp->SetBackgroundColour(foregroundColor);
		setWriteOp->SetTextColour(textColor);
		menu.Append(setWriteOp);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void
			{
				WriteMenu::WriteEntry writeEntry;
				writeEntry.address = addressPool[row];
				writeEntry.type = (ValueType)selectValueType->GetSelection();

				writeEntry.operation = (WriteMenu::Operation)wxGetSingleChoiceIndex("Chose the operation to execute", "Operation", wxArrayString(5, writeMenu->operationStrs));
				if (writeEntry.operation == -1) { return; }

				wxString valueInput = wxGetTextFromUser("Enter the value to use in the operation", "Value", "1");
				if (!valueInput.ToDouble(&writeEntry.value)) { return; }
				writeEntry.valueStr = valueInput;

				writeEntry.frequency = wxGetNumberFromUser("The operation will be executed once every this many miliseconds", "Frequency:", "Frequency", 0, 0, 100000);
				if (writeEntry.frequency == -1) { return; }

				if (selectedRows.IsEmpty())
				{
					writeMenu->AddWriteEntry(writeEntry);
				}

				for (int i = 0; i < selectedRows.GetCount(); i++)
				{
					int currentRow = selectedRows.Item(i);
					writeEntry.address = addressPool[currentRow];
					writeMenu->AddWriteEntry(writeEntry);
				}
			}, 106);
	}
	
	wxMenuItem* remove = menu.Append(102, "Remove");
	remove->SetBackgroundColour(foregroundColor);
	remove->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (selectedRows.IsEmpty()) { RemoveRow(row); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				RemoveRow(selectedRows.Item(i)-i);
			}
		}, 102);

	wxMenuItem* cpyAddr = menu.Append(103, "Copy Address");
	cpyAddr->SetBackgroundColour(foregroundColor);
	cpyAddr->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 0)); }, 103);

	wxMenuItem* cpyOff = menu.Append(104, "Copy Offset");
	cpyOff->SetBackgroundColour(foregroundColor);
	cpyOff->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 1)); }, 104);

	wxMenuItem* cpyVal = menu.Append(105, "Copy Value");
	cpyVal->SetBackgroundColour(foregroundColor);
	cpyVal->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 2)); }, 105);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x + 10, pos.y + 145));
	e.Skip();
}

void Main::RemoveRow(int row) 
{
	addrList->DeleteRows(row);
	addressPool.erase(addressPool.begin() + row);
	bytes.erase(bytes.begin() + row);

	addrList->SetCellValue(0, 1, ""); // first row's offset is nothing
	resultsTxt->SetLabel("Results: " + std::to_string(addressPool.size()));
}

void Main::OpenScanSettings(wxCommandEvent& e)
{
	scanSettingsMenu->OpenMenu(GetPosition());
}

void Main::ToggleManualUpdate(wxCommandEvent& e)
{
	if (manualUpdate->GetValue())
	{
		manualUpdateButton->Enable();
		updateTimer->Stop();
	}
	else
	{
		manualUpdateButton->Disable();
		updateTimer->Start(updateRate);
	}
}

void Main::ManuallyUpdate(wxCommandEvent& e)
{
	if (!scanning) { return; }
	
	int rows = addrList->GetNumberRows();
	int add = addressPool.size() - rows;
	if (add > 0 && rows < 1000) 
	{
		if ((add + rows) > 1000) { add = 1000 - rows; }
		addrList->AppendRows(add);
	}

	#pragma warning(suppress : 4996) // wxWidgets doesn't like creating a wxTimerEvent
	wxTimerEvent e2;
	UpdateListOnTimer(e2);
	#pragma warning(default : 4996)
}

void Main::UpdateBaseAndRoundingAndThreads() 
{
	//Get and check base
	baseInput->GetValue().ToInt(&base);
	if (base < 2 || base > 36) { base = 10; baseInput->SetValue("10"); }

	//Get and check rounding
	roundInput->GetValue().ToInt(&roundingPlace);
	if (roundingPlace < 0 || roundingPlace > 7) { roundingPlace = 2; roundInput->SetValue("2"); }

	//Get and check threads
	threadsInput->GetValue().ToInt(&threads);
	if (threads < 1 || threads > 999 || (scanning && threads > addressPool.size())) { threads = 1; threadsInput->SetValue("1"); }
}

void Main::UpdateRoundFloats(wxCommandEvent& e)
{
	if (roundFloats->IsChecked())
	{
		roundInput->SetEditable(true);
		roundInput->SetValue("2");
	}
	else
	{
		roundInput->SetEditable(false);
		roundInput->SetValue("");
	}
}

void Main::UpdateValueType(wxCommandEvent& e)
{
	ValueType type = (ValueType)selectValueType->GetSelection();
	if (type < Float) // it's an integer
	{
		selectScanType->Set(wxArrayString(numberOfFirstScanTypes, scanTypeStrs));
		selectScanType->SetSelection(Equal);
		
		baseInputLabel->SetOwnForegroundColour(textColor);
		baseInputLabel->Refresh();
		baseInput->SetEditable(true);
		baseInput->SetOwnForegroundColour(textColor);
		baseInput->Refresh();
		roundInput->SetEditable(false);
		roundInput->SetValue("");
		roundFloats->SetValue(false);
		roundFloats->Disable();
	}
	else if (type > Double) // bytes or string
	{
		selectScanType->Set(wxArrayString(1, "Equal"));
		selectScanType->SetSelection(Equal);
		
		baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInputLabel->Refresh();
		baseInput->SetEditable(false);
		baseInput->SetLabel("16");
		baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInput->Refresh();
		roundFloats->Disable();

		base = 16;
	}
	else // floats
	{
		selectScanType->Set(wxArrayString(numberOfFirstScanTypes, scanTypeStrs));
		selectScanType->SetSelection(Equal);
		
		baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInputLabel->Refresh();
		baseInput->SetEditable(false);
		baseInput->SetLabel("10");
		baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInput->Refresh();
		roundFloats->Enable();

		base = 10;
	}
}

void Main::UpdateScanType(wxCommandEvent& e)
{
	ScanType scanType = (ScanType)selectScanType->GetSelection();
	
	if (!scanning && scanType == All)
	{
		valueInput->SetEditable(false);
		valueInput->SetValue("");

		manualUpdate->SetValue(true);
		manualUpdate->Disable();
		manualUpdateButton->Disable();
		updateTimer->Stop();
	}
	else 
	{
		manualUpdate->Enable();
		manualUpdateButton->Disable();
		manualUpdate->SetValue(false);
	}

	if (scanning && scanType == All)
	{
		selectScanType->SetSelection(Equal);
	}
	
	if (scanType > DecreasedBy)
	{
		valueInput->SetEditable(false);
		valueInput->SetValue("");
	}
	else
	{
		valueInput->SetEditable(true);
	}

	if (scanType == Between) 
	{
		valueInput->SetMinSize(wxSize(235, 25));
		valueInput->SetMaxSize(wxSize(235, 25));
		valueInput2->Show();
		Layout();
	}
	else 
	{
		valueInput->SetMinSize(wxSize(9999, 25));
		valueInput->SetMaxSize(wxSize(9999, 25));
		valueInput2->Hide();
		Layout();
	}
} 

void Main::CloseApp(wxCloseEvent& e)
{
	selectProcMenu->Destroy();

	scanSettingsMenu->Destroy();

	delete savedMenu->updateTimer;
	savedMenu->Destroy();

	wxCommandEvent e2; breakpointMenu->DetachDebugger(e2);
	breakpointMenu->Destroy();

	hexCalculator->Destroy();

	delete writeMenu->writeTimer;
	writeMenu->Destroy();

	delete updateTimer;
	delete keyInputTimer;

	Destroy();
}


// select proc menu

wxBEGIN_EVENT_TABLE(SelectProcessMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TEXT(ProcessNameInputID, SearchProcessList)
EVT_CHECKBOX(UseDebugPrivilegeID, UpdateDebugPrivilege)
EVT_LISTBOX_DCLICK(ProcessListID, SelectProcess)
wxEND_EVENT_TABLE()

SelectProcessMenu::SelectProcessMenu(Main* mainPtr) : wxFrame(nullptr, MainWindowID, "Select Process", wxPoint(50, 50), wxSize(400, 400))
{
	main = mainPtr;

	SetOwnBackgroundColour(backgroundColor);

	processNameInput = new wxTextCtrl(this, ProcessNameInputID, "", wxPoint(0, 0), wxSize(9999, 25));
	processNameInput->SetOwnBackgroundColour(foregroundColor);
	processNameInput->SetOwnForegroundColour(textColor);

	useDebugPrivilege = new wxCheckBox(this, UseDebugPrivilegeID, "Use Debug Privilege (must be running as administrator to use)");
	useDebugPrivilege->SetOwnForegroundColour(textColor);

	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) 
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) 
		{
			if (Elevation.TokenIsElevated == FALSE) 
			{
				useDebugPrivilege->Disable();
			}
		}
	}
	if (hToken) { CloseHandle(hToken); }

	processList = new wxListBox(this, ProcessListID, wxPoint(0, 0), wxSize(9999, 9999));
	processList->SetOwnBackgroundColour(foregroundColor);
	processList->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(processNameInput, 0, wxALL, 10);
	vSizer->Add(useDebugPrivilege, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);
	vSizer->Add(processList, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);
}

void SelectProcessMenu::RefreshProcessList()
{
	processList->Clear();

	originalProcIds.clear();
	originalProcIds.shrink_to_fit();

	std::vector<std::wstring> names;
	int count = 0;
	HANDLE procSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
	if (procSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(procSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, L"svchost.exe")) { continue; }

				names.push_back(std::wstring(procEntry.szExeFile) + L" (" + std::to_wstring(procEntry.th32ProcessID) + L")");
				originalProcIds.push_back(procEntry.th32ProcessID);

				count++;
			} while (Process32Next(procSnap, &procEntry));
		}
	}
	CloseHandle(procSnap);

	std::vector<const wchar_t*> cStrArray;
	cStrArray.reserve(names.size());
	for (int i = 0; i < names.size(); i++)
	{
		cStrArray.push_back(names[i].c_str());
	}

	processNames = new wxArrayString(count, cStrArray.data());
	processList->InsertItems(*processNames, 0);

	currentProcIds = originalProcIds;
}

void SelectProcessMenu::SearchProcessList(wxCommandEvent& e)
{
	wxString input = processNameInput->GetValue().Lower();
	unsigned int inputLen = input.Length();

	if (input.IsEmpty())
	{
		RefreshProcessList();
		return;
	}

	processList->Clear();

	currentProcIds.clear();
	currentProcIds.shrink_to_fit();

	int lastIndex = 0;

	for (int i = 0; i < processNames->GetCount(); i++)
	{
		wxString currentProcessName = processNames->Item(i).Lower();
		unsigned int currentNameLen = currentProcessName.Length();

		for (int j = 0; j < currentNameLen; j++)
		{
			for (int k = 0; k < inputLen; k++)
			{
				if (currentProcessName.GetChar(j + k) != input.GetChar(k))
				{
					break;
				}
				else if (k == inputLen - 1)
				{
					processList->InsertItems(1, &(processNames->Item(i)), lastIndex);
					currentProcIds.push_back(originalProcIds[i]);
					lastIndex++;
				}
			}
		}
	}
}

void SelectProcessMenu::SelectProcess(wxCommandEvent& e)
{
	int selection = processList->GetSelection();
	
	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, currentProcIds[selection]);

	if (procHandle) 
	{
		main->UpdateProcessSelection(procHandle, processList->GetString(selection));

		// close this menu
		wxCloseEvent e2;
		CloseMenu(e2);
	}
	else
	{
		wxMessageBox("Failed to open process. You may need to use debug privilege", "Failed to open process");
	}
}

void SelectProcessMenu::UpdateDebugPrivilege(wxCommandEvent& e)
{
	LUID luid;
	LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid);

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (useDebugPrivilege->IsChecked())
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // enable
	}
	else 
	{
		tp.Privileges[0].Attributes = 0; // disable
	}

	HANDLE accessToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &accessToken);
	
	AdjustTokenPrivileges(accessToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);
}

void SelectProcessMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	RefreshProcessList();
}

void SelectProcessMenu::CloseMenu(wxCloseEvent& e)
{
	Hide();

	delete processNames;

	originalProcIds.clear();
	originalProcIds.shrink_to_fit();
}