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
EVT_BUTTON(OpenSavedListID, OpenSavedMenu)
EVT_BUTTON(OpenBreakpointMenuID, OpenBreakpointMenu)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, MainWindowID, "Hex Investigator x64", wxPoint(50, 50), wxSize(550, 650))
{
	SetOwnBackgroundColour(wxColour(35, 35, 35));

	selectProc = new wxButton(this, SelectProcessID, "P", wxPoint(0, 0), wxSize(25, 25));
	selectProc->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectProc->SetOwnForegroundColour(wxColour(220, 220, 220));

	selectProcMenu = new SelectProcessMenu(this);

	selectValueType = new wxChoice(this, SelectValueTypeID, wxPoint(0, 0), wxSize(70, 50), wxArrayString(11, typeStrs));
	selectValueType->SetSelection(1);
	selectValueType->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectValueType->SetOwnForegroundColour(wxColour(220, 220, 220));

	selectScanType = new wxChoice(this, SelectScanTypeID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(7, firstScans));
	selectScanType->SetSelection(0);
	selectScanType->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectScanType->SetOwnForegroundColour(wxColour(220, 220, 220));

	baseInputLabel = new wxStaticText(this, wxID_ANY, "Base:");
	baseInputLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	baseInput = new wxTextCtrl(this, wxID_ANY, "10", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	baseInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	baseInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	roundFloats = new wxCheckBox(this, RoundFloatsID, "Round to Place:");
	roundFloats->SetOwnForegroundColour(wxColour(220, 220, 220));
	roundFloats->Disable();

	roundInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	roundInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	roundInput->SetOwnForegroundColour(wxColour(220, 220, 220));
	roundInput->SetEditable(false);

	threadsInputLabel = new wxStaticText(this, wxID_ANY, "Threads:");
	threadsInputLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	threadsInput = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(0, 0), wxSize(30, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	threadsInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	threadsInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	firstScan = new wxButton(this, FirstScanID, "First Scan", wxPoint(0, 0), wxSize(75, 25));
	firstScan->SetOwnBackgroundColour(wxColour(60, 60, 60));
	firstScan->SetOwnForegroundColour(wxColour(220, 220, 220));
	firstScan->Disable();

	nextScan = new wxButton(this, NextScanID, "Next Scan", wxPoint(0, 0), wxSize(75, 25));
	nextScan->SetOwnBackgroundColour(wxColour(60, 60, 60));
	nextScan->SetOwnForegroundColour(wxColour(220, 220, 220));
	nextScan->Disable();

	scanSettingsMenu = new ScanSettingsMenu(procHandle);

	scanSettingsButton = new wxButton(this, ScanSettingsID, "Scan Settings", wxPoint(0, 0), wxSize(100, 25));
	scanSettingsButton->SetOwnBackgroundColour(wxColour(60, 60, 60));
	scanSettingsButton->SetOwnForegroundColour(wxColour(220, 220, 220));
	scanSettingsButton->Disable();

	valueInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(9999, 25));
	valueInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	valueInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	manualUpdate = new wxCheckBox(this, ManualUpdateToggleID, "Manually Update List");
	manualUpdate->SetOwnForegroundColour(wxColour(220, 220, 220));

	manualUpdateButton = new wxButton(this, ManualUpdateButtonID, "Update", wxPoint(0, 0), wxSize(75, 25));
	manualUpdateButton->SetOwnBackgroundColour(wxColour(60, 60, 60));
	manualUpdateButton->SetOwnForegroundColour(wxColour(220, 220, 220));
	manualUpdateButton->Disable();

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(wxColour(40, 40, 40));
	addrList->SetLabelTextColour(wxColour(230, 230, 230));
	addrList->SetDefaultCellBackgroundColour(wxColour(60, 60, 60));
	addrList->SetDefaultCellTextColour(wxColour(220, 220, 220));

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
	resultsTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	savedMenu = new SavedMenu(procHandle);

	openSaved = new wxButton(this, OpenSavedListID, "Open Saved List", wxPoint(0, 0), wxSize(115, 25));
	openSaved->SetOwnBackgroundColour(wxColour(60, 60, 60));
	openSaved->SetOwnForegroundColour(wxColour(220, 220, 220));
	openSaved->Disable();

	breakpointMenu = new BreakpointMenu(procHandle);

	openBreakpointer = new wxButton(this, OpenBreakpointMenuID, "Open Breakpoint Menu", wxPoint(0, 0), wxSize(135, 25));
	openBreakpointer->SetOwnBackgroundColour(wxColour(60, 60, 60));
	openBreakpointer->SetOwnForegroundColour(wxColour(220, 220, 220));
	openBreakpointer->Disable();

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
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

	row3Sizer->Add(resultsTxt, 0, wxLEFT, 10);
	row3Sizer->AddStretchSpacer(1);
	row3Sizer->Add(openBreakpointer, 0, wxRIGHT, 10);
	row3Sizer->Add(openSaved, 0, wxRIGHT, 10);
	
	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(valueInput, 0, wxEXPAND | wxALL, 10);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(addrList, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	SetSizer(vSizer);

	updateTimer = new wxTimer(this, UpdateTimerID);

	keyInputTimer = new wxTimer(this, KeyInputTimerID);
	keyInputTimer->Start(100);
}

// memory scan functions

template <typename T> unsigned int Main::FirstScan(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<unsigned long long> newAddresses;
	std::vector<unsigned char> newBytes;
	
	unsigned long long baseAddress = scanSettings.minAddress;

	int size = sizeof(T);

	int offset = 1;
	if (scanSettings.alignMemory) { offset = size; }

	// the use of + converts it to a function ptr so the variable meetsCondition can be reassigned
	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal
	switch (scanSettings.scanType)
	{
	case 1: // Greater
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case 2: // Less
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case 3: // Not Equal
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case 4: // Greater or Equal
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case 5: // Less or Equal
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (unsigned long long*)baseAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			unsigned char* buffer = new unsigned char[mbi.RegionSize];

			ReadProcessMemory(procHandle, mbi.BaseAddress, buffer, mbi.RegionSize, 0);

			for (int i = 0; i < mbi.RegionSize; i += offset)
			{
				T value = *(T*)(buffer + i);

				if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

				if (meetsCondition(value, targetValue))
				{
					newAddresses.push_back(baseAddress + i);

					unsigned char* valueBytes = (unsigned char*)&value;
					newBytes.insert(newBytes.end(), valueBytes, valueBytes + size);

					results++;
				}
			}

			delete[] buffer;
		}

		baseAddress += mbi.RegionSize;
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

unsigned int Main::FirstScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<unsigned long long>* addressesPtr)
{
	std::vector<unsigned long long> newAddresses;
	
	unsigned long long baseAddress = scanSettings.minAddress;

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (unsigned long long*)baseAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			unsigned char* buffer = new unsigned char[mbi.RegionSize];

			ReadProcessMemory(procHandle, mbi.BaseAddress, buffer, mbi.RegionSize, 0);

			for (int i = 0; i < mbi.RegionSize; i++)
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

		baseAddress += mbi.RegionSize;
	}

	*addressesPtr = newAddresses;

	return results;
}

template <typename T> unsigned int Main::FirstScanAll(MemoryScanSettings scanSettings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<unsigned long long> newAddresses;
	std::vector<unsigned char> newBytes;
	
	unsigned long long baseAddress = scanSettings.minAddress;

	int offset = 1;
	if (scanSettings.alignMemory) { offset = sizeof(T); }

	unsigned int results = 0;

	_MEMORY_BASIC_INFORMATION mbi;
	while (baseAddress < scanSettings.maxAddress && VirtualQueryEx(procHandle, (unsigned long long*)baseAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT &&
			(scanSettings.protection == -1 || mbi.Protect == scanSettings.protection) &&
			((scanSettings.scanImageMem && mbi.Type == MEM_IMAGE) || (scanSettings.scanMappedMem && mbi.Type == MEM_MAPPED) || (scanSettings.scanPrivateMem && mbi.Type == MEM_PRIVATE)))
		{
			newAddresses.push_back(baseAddress); // store the base address
			newAddresses.push_back(mbi.RegionSize); // store the size
			
			unsigned char* buffer = new unsigned char[mbi.RegionSize];
			ReadProcessMemory(procHandle, mbi.BaseAddress, buffer, mbi.RegionSize, 0);
			newBytes.insert(newBytes.end(), buffer, buffer + mbi.RegionSize); // store data

			delete[] buffer;

			results += mbi.RegionSize / offset;
		}

		baseAddress += mbi.RegionSize;
	}

	*addressesPtr = newAddresses;
	*bytesPtr = newBytes;

	return results;
}

template <typename T> unsigned int Main::NextScan(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr)
{
	std::vector<unsigned long long> newAddresses;
	std::vector<unsigned char> newBytes;

	int size = sizeof(T);

	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal, Increased By, Decreased By, Unchanged
	switch (scanSettings.scanType)
	{
	case 1: // Greater 
	case 8: // Increased
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case 2: // Less
	case 9: // Decreased
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case 3: // Not Equal
	case 10: // Changed
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case 4: // Greater or Equal
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case 5: // Less or Equal
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned char* dataPtr = bytes.data();

	int results = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i++)
	{
		T value;
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], &value, size, 0);

		if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

		T otherValue = targetValue;

		if (scanSettings.scanType > 5) // scan types that need the old value
		{
			otherValue = *(T*)(dataPtr + (i * size)); // set to last value

			if (scanSettings.scanType == 6) { otherValue += targetValue; } // checking increase by targetValue
			else if (scanSettings.scanType == 7) { otherValue -= targetValue; } // checking decrease by targetValue
		}

		if (meetsCondition(value, otherValue))
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

unsigned int Main::NextScanByteArray(MemoryScanSettings scanSettings, unsigned char* targetBytes, int size, std::vector<unsigned long long>* addressesPtr)
{
	std::vector<unsigned long long> newAddresses;

	unsigned int results = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i++)
	{
		unsigned char* value = new unsigned char[size];
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], value, size, 0);

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

template <typename T> unsigned int Main::NextScanAll(MemoryScanSettings scanSettings, T targetValue, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) // this function is called after FirstScanAll because of the different way the addresses are stored
{
	std::vector<unsigned long long> newAddresses;
	std::vector<unsigned char> newBytes;

	int size = sizeof(T);

	int offset = 1;
	if (scanSettings.alignMemory) { offset = size; }

	auto meetsCondition = +[](T x, T y) { return x == y; }; // Equal, Increased By, Decreased By, Unchanged
	switch (scanSettings.scanType)
	{
	case 1: // Greater 
	case 8: // Increased
		meetsCondition = +[](T x, T y) { return x > y; };
		break;
	case 2: // Less
	case 9: // Decreased
		meetsCondition = +[](T x, T y) { return x < y; };
		break;
	case 3: // Not Equal
	case 10: // Changed
		meetsCondition = +[](T x, T y) { return x != y; };
		break;
	case 4: // Greater or Equal
		meetsCondition = +[](T x, T y) { return x >= y; };
		break;
	case 5: // Less or Equal
		meetsCondition = +[](T x, T y) { return x <= y; };
		break;
	}

	unsigned char* dataPtr = bytes.data();

	unsigned int results = 0;

	int byteIndex = 0;

	for (int i = scanSettings.minAddress; i < scanSettings.maxAddress; i += 2) // after FirstScanAll, region bases and the size are stored in pairs
	{
		unsigned long long address = addressPool[i];
		int regionSize = addressPool[i + 1];
		for (int j = 0; j < regionSize; j += offset)
		{
			T value;
			ReadProcessMemory(procHandle, (unsigned long long*)(address + j), &value, size, 0);

			if (scanSettings.roundFloats) { value = std::round(value * scanSettings.roundingValue) / scanSettings.roundingValue; }

			T otherValue = targetValue;

			if (scanSettings.scanType > 5) // scan types that need the old value
			{ 
				otherValue = *(T*)(dataPtr + byteIndex); // set to last value

				if (scanSettings.scanType == 6) { otherValue += targetValue; } // checking increase by targetValue
				else if (scanSettings.scanType == 7) { otherValue -= targetValue; } // checking decrease by targetValue
			}

			if (meetsCondition(value, otherValue))
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
		selectScanType->GetSelection(),
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
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], &value, sizeof(T), 0);

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
			valueStr = std::string(_itoa(value, buffer, base));
		}

		addrList->SetCellValue(i, 2, valueStr);

		if (i != 0)
		{
			std::stringstream offsetToHex;
			offsetToHex << std::hex << (addressPool[i] - addressPool[i - 1]);
			addrList->SetCellValue(i, 1, offsetToHex.str());
		}

		if (addrList->GetCellValue(i, 0) != "") { continue; }

		AddressModuleInfo info = GetAddressModuleInfo(addressPool[i]);

		std::stringstream addressToHex;

		if (info.rva != 0) // it's an address that is part of a module
		{
			addressToHex << std::hex << info.rva;

			addrList->SetCellValue(i, 0, info.moduleName + "+" + addressToHex.str() + " " + info.sectionName);

			switch (info.secitonType) 
			{
			case info.SectionType::Code:
				addrList->SetCellTextColour(i, 0, wxColour(255, 0, 0));
				break;
			case info.SectionType::InitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 255, 0));
				break;
			case info.SectionType::UninitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 150, 255));
				break;
			}
		}
		else 
		{
			addressToHex << std::hex << addressPool[i];

			addrList->SetCellValue(i, 0, addressToHex.str());
		}
	}
}

void Main::UpdateListByteArray(int size)
{
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }

		unsigned char* value = new unsigned char[size];
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], value, size, 0);

		std::string valueStr;
		for (int i = 0; i < size; i++)
		{
			std::stringstream byteToHex;
			byteToHex << std::hex << (int)value[i];
			valueStr += byteToHex.str() + ' ';
		}

		delete[] value;

		addrList->SetCellValue(i, 2, valueStr);

		if (i != 0)
		{
			std::stringstream offsetToHex;
			offsetToHex << std::hex << (addressPool[i] - addressPool[i - 1]);
			addrList->SetCellValue(i, 1, offsetToHex.str());
		}

		if (addrList->GetCellValue(i, 0) != "") { continue; }

		AddressModuleInfo info = GetAddressModuleInfo(addressPool[i]);

		std::stringstream addressToHex;

		if (info.rva != 0) // it's an address that is part of a module
		{
			addressToHex << std::hex << info.rva;

			addrList->SetCellValue(i, 0, info.moduleName + "+" + addressToHex.str() + " " + info.sectionName);

			switch (info.secitonType)
			{
			case info.SectionType::Code:
				addrList->SetCellTextColour(i, 0, wxColour(255, 0, 0));
				break;
			case info.SectionType::InitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 255, 0));
				break;
			case info.SectionType::UninitData:
				addrList->SetCellTextColour(i, 0, wxColour(0, 150, 255));
				break;
			}
		}
		else
		{
			addressToHex << std::hex << addressPool[i];

			addrList->SetCellValue(i, 0, addressToHex.str());
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

Main::AddressModuleInfo Main::GetAddressModuleInfo(unsigned long long address)
{
	AddressModuleInfo info = {};
	
	for (int i = 0; i < moduleHandles.size(); i++)
	{
		if (address < moduleHandles[i]) { continue; }

		unsigned long long relativeAddress = address - moduleHandles[i];

		//https://reverseengineering.stackexchange.com/questions/6077/get-sections-names-and-headers-for-a-file-using-c

		IMAGE_DOS_HEADER dosHeader = {};
		ReadProcessMemory(procHandle, (unsigned long long*)moduleHandles[i], &dosHeader, sizeof(dosHeader), nullptr);

		unsigned long long imageNtHeadersAddress = (moduleHandles[i] + (unsigned long long)dosHeader.e_lfanew);
		
		IMAGE_NT_HEADERS imageNtHeader = {};
		ReadProcessMemory(procHandle, (unsigned long long*)imageNtHeadersAddress, &imageNtHeader, sizeof(imageNtHeader), nullptr);

		for (int j = 0; j < imageNtHeader.FileHeader.NumberOfSections; j++)
		{
			IMAGE_SECTION_HEADER section = {};

			unsigned long long sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * j) + imageNtHeadersAddress + sizeof(imageNtHeader.Signature) + sizeof(imageNtHeader.FileHeader) + imageNtHeader.FileHeader.SizeOfOptionalHeader;
			
			ReadProcessMemory(procHandle, (unsigned long long*)sectionAddress, &section, sizeof(section), nullptr);
			
			DWORD sectionBase = section.VirtualAddress;
			DWORD sectionEnd = sectionBase + section.Misc.VirtualSize;
			
			if (relativeAddress >= sectionBase && relativeAddress <= sectionEnd)
			{
				info.rva = relativeAddress;

				if (section.Characteristics & IMAGE_SCN_CNT_CODE) { info.secitonType = info.SectionType::Code; }
				else if (section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) { info.secitonType = info.SectionType::InitData; }
				else if (section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) { info.secitonType = info.SectionType::UninitData; }

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
				moduleHandles.push_back((unsigned long long)modEntry.hModule);
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

// gui functions

void Main::CheckKeyInput(wxTimerEvent& e)
{
	if(scanSettingsMenu->scanKeybind == -1) { return; }
	
	wxCommandEvent empty = 0;
	
	if (GetAsyncKeyState(scanSettingsMenu->scanKeybind) & 1) // K
	{
		if (scanning) { NextScanButtonPress(empty); }
		else { FirstScanButtonPress(empty); }
	}
}

void Main::OpenSelectProcessMenu(wxCommandEvent& e)
{
	selectProcMenu->OpenMenu(GetPosition());
}

void Main::UpdateProcessSelection(DWORD procId) 
{
	ResetScan();

	procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	savedMenu->procHandle = procHandle;
	breakpointMenu->procHandle = procHandle;
	scanSettingsMenu->procHandle = procHandle;

	firstScan->Enable();
	scanSettingsButton->Enable();
	openSaved->Enable();
	openBreakpointer->Enable();
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

	bool freezeDuringScan = scanSettingsMenu->freezeProcess->IsChecked();

	if (freezeDuringScan) { FreezeProcess(true); }

	MemoryScanSettings memoryScanSettings = CreateScanSettingsStruct();
	unsigned long long originalMax = memoryScanSettings.maxAddress;
	
	performedAllScan = selectScanType->GetSelection() == 6;

	unsigned int results = 0;

	int valueType = selectValueType->GetSelection();

	std::vector<std::thread> scanThreads(threads);

	std::vector<std::vector<unsigned long long>> addressLists(threads);
	std::vector<std::vector<unsigned char>> byteLists(threads);

	for (int i = 0; i < threads; i++) 
	{
		memoryScanSettings.maxAddress = originalMax * ((i+1) / (float)threads);

		switch (valueType)
		{
		case 0:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<long long>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			long long targetValue = 0;
			if (!valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, long long targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<long long>(settings, targetValue, addressesPtr, bytesPtr); },
				this, targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 1:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<int>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, int targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<int>(settings, targetValue, addressesPtr, bytesPtr); },
				this, targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 2:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<short>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, short targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<short>(settings, targetValue, addressesPtr, bytesPtr); },
				this, (short)targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 3:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<char>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			int targetValue = 0;
			if (!valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, char targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<char>(settings, targetValue, addressesPtr, bytesPtr); },
				this, (char)targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 4:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned long long>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned long long targetValue = 0;
			if (!valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned long long targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned long long>(settings, targetValue, addressesPtr, bytesPtr); },
				this, targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 5:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned int>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned int targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned int>(settings, targetValue, addressesPtr, bytesPtr); },
				this, targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 6:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned short>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned short targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned short>(settings, targetValue, addressesPtr, bytesPtr); },
				this, (unsigned short)targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 7:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<unsigned char>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			unsigned int targetValue = 0;
			if (!valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned char targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<unsigned char>(settings, targetValue, addressesPtr, bytesPtr); },
				this, (unsigned char)targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 8:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<float>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			double targetValue = 0;
			if (!valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, float targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<float>(settings, targetValue, addressesPtr, bytesPtr); },
				this, (float)targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 9:
		{
			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->FirstScanAll<double>(settings, addressesPtr, bytesPtr); },
					this, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
				break;
			}

			double targetValue = 0;
			if (!valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, double targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->FirstScan<double>(settings, targetValue, addressesPtr, bytesPtr); },
				this, targetValue, &results, memoryScanSettings, &addressLists[i], &byteLists[i]);
			break;
		}
		case 10:
		{
			unsigned char* targetValue;
			if (!ParseByteArray(valueInput->GetValue(), &targetValue)) { wxMessageBox("Invalid Value. Should be in format FB23EA...", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr) -> void
				{ *results += main->FirstScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		}

		memoryScanSettings.minAddress = memoryScanSettings.maxAddress;
	}

	for (int i = 0; i < threads; i++) 
	{
		scanThreads[i].join(); // wait for thread to finish

		addressPool.insert(addressPool.end(), addressLists[i].begin(), addressLists[i].end());
		addressLists[i].clear(); addressLists[i].shrink_to_fit();

		bytes.insert(bytes.end(), byteLists[i].begin(), byteLists[i].end());
		byteLists[i].clear(); byteLists[i].shrink_to_fit();
	}

	if (results > 1000)
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results) + " (1000 shown)");
		results = 1000;
	}
	else
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results));
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

	if (valueType == 10) { return; } // dont change the scan options if the type is a byte array
	selectScanType->Set(wxArrayString(12, nextScans));
	selectScanType->SetSelection(0);
	valueInput->Show();
}

void Main::NextScanButtonPress(wxCommandEvent& e)
{
	if (manualUpdate->IsChecked()) { updateTimer->Stop(); }
	UpdateBaseAndRoundingAndThreads();

	bool noInput = selectScanType->GetSelection() > 7; // input will not be read if scan type is Increased, Decreased, Changed, or Unchanged

	bool freezeDuringScan = scanSettingsMenu->freezeProcess->IsChecked();

	if (freezeDuringScan) { FreezeProcess(true); }

	MemoryScanSettings memoryScanSettings = CreateScanSettingsStruct();
	unsigned long long originalMax = addressPool.size();

	unsigned int results = 0;

	std::vector<std::thread> scanThreads(threads);

	std::vector<std::vector<unsigned long long>> addressLists(threads);
	std::vector<std::vector<unsigned char>> byteLists(threads);

	int valueType = selectValueType->GetSelection();

	for (int i = 0; i < threads; i++) 
	{
		memoryScanSettings.minAddress = originalMax * (i / (float)threads);
		memoryScanSettings.maxAddress = originalMax * ((i+1) / (float)threads);
		if (performedAllScan && memoryScanSettings.minAddress % 2 != 0) { memoryScanSettings.minAddress--; } // the addresses are stored at even indexes while the region size is at the odd index
		if (performedAllScan && memoryScanSettings.maxAddress % 2 != 0) { memoryScanSettings.maxAddress--; }
		
		switch (valueType)
		{
		case 0:
		{
			long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan) 
			{ 
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, long long targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<long long>(settings, targetValue, addressesPtr, bytesPtr); }, 
					this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, long long targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<long long>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 1:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, int targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<int>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, int targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<int>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 2:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, short targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<short>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (short)targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, short targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<short>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (short)targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 3:
		{
			int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, char targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<char>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (char)targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, char targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<char>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (char)targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 4:
		{
			unsigned long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned long long targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned long long>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned long long targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned long long>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 5:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned int targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned int>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned int targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned int>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 6:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned short targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned short>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (unsigned short)targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned short targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned short>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (unsigned short)targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 7:
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned char targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<unsigned char>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (unsigned char)targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, unsigned char targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<unsigned char>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (unsigned char)targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 8:
		{
			double targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, float targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<float>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, (float)targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, float targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<float>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, (float)targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 9:
		{
			double targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }

			if (performedAllScan)
			{
				scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, double targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
					{ *results += main->NextScanAll<double>(settings, targetValue, addressesPtr, bytesPtr); },
					this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
				break;
			}

			scanThreads[i] = std::thread([](Main* main, MemoryScanSettings settings, double targetValue, unsigned int* results, std::vector<unsigned long long>* addressesPtr, std::vector<unsigned char>* bytesPtr) -> void
				{ *results += main->NextScan<double>(settings, targetValue, addressesPtr, bytesPtr); },
				this, memoryScanSettings, targetValue, &results, &addressLists[i], &byteLists[i]);
			break;
		}
		case 10:
		{
			unsigned char* targetValue;
			if (!ParseByteArray(valueInput->GetValue(), &targetValue)) { wxMessageBox("Invalid Value. Should be in format FB23EA...", "Can't Scan"); return; }

			scanThreads[i] = std::thread([](Main* main, unsigned char* targetValue, unsigned int* results, MemoryScanSettings settings, std::vector<unsigned long long>* addressesPtr) -> void
				{ *results += main->NextScanByteArray(settings, targetValue, main->byteArraySize, addressesPtr); delete[] targetValue; },
				this, targetValue, &results, memoryScanSettings, &addressLists[i]);
			break;
		}
		}
	}

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
	
	if (results > 1000)
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results) + " (1000 shown)");
		results = 1000;
	}
	else
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results));
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
	int type = selectValueType->GetSelection();
	switch (type)
	{
	case 0:
		UpdateList<long long>(false);
		break;
	case 1:
		UpdateList<int>(false);
		break;
	case 2:
		UpdateList<short>(false);
		break;
	case 3:
		UpdateList<char>(false);
		break;
	case 4:
		UpdateList<unsigned long long>(false);
		break;
	case 5:
		UpdateList<unsigned int>(false);
		break;
	case 6:
		UpdateList<unsigned short>(false);
		break;
	case 7:
		UpdateList<unsigned char>(false);
		break;
	case 8:
		UpdateList<float>(true);
		break;
	case 9:
		UpdateList<double>(true);
		break;
	case 10:
		UpdateListByteArray(byteArraySize);
		break;
	}
}

void Main::WriteValueHandler(wxString input, unsigned long long* address)
{
	if (input == "") { return; }

	//Get and check base
	baseInput->GetValue().ToInt(&base);
	if (base < 2 || base > 36) { base = 10; baseInput->SetValue("10"); }
	
	int type = selectValueType->GetSelection();
	switch (type)
	{
		case 0:
		{
			long long targetValue = 0;
			if (!input.ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 1:
		{
			int targetValue = 0;
			if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 2:
		{
			int targetValue = 0;
			if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 3:
		{
			int targetValue = 0;
			if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 4:
		{
			unsigned long long targetValue = 0;
			if (!input.ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 5:
		{
			unsigned int targetValue = 0;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 6:
		{
			unsigned int targetValue = 0;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 7:
		{
			unsigned int targetValue = 0;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 8:
		{
			double targetValue = 0;
			if (!input.ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
		case 9:
		{
			double targetValue = 0;
			if (!input.ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			WriteValue(procHandle, address, &targetValue, sizeof(targetValue));
			break;
		}
	}
}

void Main::ResetScan()
{
	nextScan->Disable();
	selectValueType->Enable();
	valueInput->Enable();

	selectScanType->Set(wxArrayString(7, firstScans));
	selectScanType->SetSelection(0);

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
	save->SetBackgroundColour(wxColour(60, 60, 60));
	save->SetTextColour(wxColour(220, 220, 220));
	menu.Append(save);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (!savedMenu->IsVisible()) { savedMenu->OpenMenu(GetPosition()); }

			if (selectedRows.IsEmpty()) { savedMenu->AddAddress(addressPool[row], selectValueType->GetSelection(), base, byteArraySize); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				savedMenu->AddAddress(addressPool[selectedRows.Item(i)], selectValueType->GetSelection(), base, byteArraySize);
			}

		}, 100);

	if (selectValueType->GetSelection() != 10) // cant write if its a byte array
	{
		wxMenuItem* write = menu.Append(101, "Write Value");
		write->SetBackgroundColour(wxColour(60, 60, 60));
		write->SetTextColour(wxColour(220, 220, 220));
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
			{ 
				wxString input = wxGetTextFromUser("New Value (using current base):", "Write Value");
				
				if (selectedRows.IsEmpty()) { WriteValueHandler(input, (unsigned long long*)addressPool[row]); }

				for (int i = 0; i < selectedRows.GetCount(); i++)
				{
					WriteValueHandler(input, (unsigned long long*)addressPool[selectedRows.Item(i)]);
				}
			}, 101);
	}
	
	wxMenuItem* remove = menu.Append(102, "Remove");
	remove->SetBackgroundColour(wxColour(60, 60, 60));
	remove->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (selectedRows.IsEmpty()) { RemoveRow(row); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				RemoveRow(selectedRows.Item(i)-i);
			}
		}, 102);

	wxMenuItem* cpyAddr = menu.Append(103, "Copy Address");
	cpyAddr->SetBackgroundColour(wxColour(60, 60, 60));
	cpyAddr->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 0)); }, 103);

	wxMenuItem* cpyOff = menu.Append(104, "Copy Offset");
	cpyOff->SetBackgroundColour(wxColour(60, 60, 60));
	cpyOff->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 1)); }, 104);

	wxMenuItem* cpyVal = menu.Append(105, "Copy Value");
	cpyVal->SetBackgroundColour(wxColour(60, 60, 60));
	cpyVal->SetTextColour(wxColour(220, 220, 220));
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

void Main::OpenSavedMenu(wxCommandEvent& e)
{
	savedMenu->OpenMenu(GetPosition());
}

void Main::OpenBreakpointMenu(wxCommandEvent& e)
{
	breakpointMenu->OpenMenu(GetPosition());
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
	if (threads < 1 || threads > 999 || (scanning && threads > addressPool.size())) { threads = 1; baseInput->SetValue("1"); }
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
	int type = selectValueType->GetSelection();
	if (type < 8) // it's an integer
	{
		selectScanType->Set(wxArrayString(7, firstScans));
		selectScanType->SetSelection(0);
		
		baseInputLabel->SetOwnForegroundColour(wxColour(220, 220, 220));
		baseInputLabel->Refresh();
		baseInput->SetEditable(true);
		baseInput->SetLabel("10");
		baseInput->SetOwnForegroundColour(wxColour(220, 220, 220));
		baseInput->Refresh();
		roundInput->SetEditable(false);
		roundInput->SetValue("");
		roundFloats->SetValue(false);
		roundFloats->Disable();

		base = 10;
	}
	else if (type == 10) // bytes
	{
		selectScanType->Set(wxArrayString(1, "Equal"));
		selectScanType->SetSelection(0);
		
		baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInputLabel->Refresh();
		baseInput->SetEditable(false);
		baseInput->SetLabel("16");
		baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInput->Refresh();
		roundFloats->Disable();

		base = 16;
	}
	else //floats
	{
		selectScanType->Set(wxArrayString(7, firstScans));
		selectScanType->SetSelection(0);
		
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
	if (!scanning && selectScanType->GetSelection() == 6) 
	{
		valueInput->SetEditable(false);
		valueInput->SetValue("");

		manualUpdate->SetValue(true);
		manualUpdate->Disable();
		manualUpdateButton->Disable();
		updateTimer->Stop();
	}
	
	if (selectScanType->GetSelection() > 7)
	{
		valueInput->SetEditable(false);
		valueInput->SetValue("");
	}
	else
	{
		valueInput->SetEditable(true);
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

	delete updateTimer;
	delete keyInputTimer;

	Destroy();
}


// select proc menu

wxBEGIN_EVENT_TABLE(SelectProcessMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TEXT(ProcessNameInputID, SearchProcessList)
EVT_LISTBOX_DCLICK(ProcessListID, SelectProcess)
wxEND_EVENT_TABLE()

SelectProcessMenu::SelectProcessMenu(Main* mainPtr) : wxFrame(nullptr, MainWindowID, "Select Process", wxPoint(50, 50), wxSize(400, 400))
{
	main = mainPtr;

	SetOwnBackgroundColour(wxColour(35, 35, 35));

	processNameInput = new wxTextCtrl(this, ProcessNameInputID, "", wxPoint(0, 0), wxSize(9999, 25));
	processNameInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	processNameInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	processList = new wxListBox(this, ProcessListID, wxPoint(0, 0), wxSize(9999, 9999));
	processList->SetOwnBackgroundColour(wxColour(60, 60, 60));
	processList->SetOwnForegroundColour(wxColour(220, 220, 220));

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(processNameInput, 0, wxALL, 10);
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
	wxString input = processNameInput->GetValue();
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
		unsigned int currentNameLen = processNames->Item(i).Length();

		for (int j = 0; j < currentNameLen; j++)
		{
			for (int k = 0; k < inputLen; k++)
			{
				if (processNames->Item(i).GetChar(j + k) != input.GetChar(k))
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
	main->UpdateProcessSelection(currentProcIds[processList->GetSelection()]);

	// close 
	wxCloseEvent* e2 = (wxCloseEvent*)0;
	CloseMenu(*e2);
}

void SelectProcessMenu::OpenMenu(wxPoint position)
{
	SetPosition(position);
	Show();

	RefreshProcessList();
}

void SelectProcessMenu::CloseMenu(wxCloseEvent& e)
{
	Hide();

	delete processNames;

	originalProcIds.clear();
	originalProcIds.shrink_to_fit();
}