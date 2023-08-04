#include "Main.h"

wxBEGIN_EVENT_TABLE(Main, wxFrame)
EVT_CLOSE(CloseWrapper)
EVT_BUTTON(1, SelectProcess)
EVT_CHOICE(2, UpdateType)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_CHOICE(8, UpdateScan)
EVT_BUTTON(5, FirstScanWrapper)
EVT_BUTTON(6, NextScanWrapper)
EVT_TIMER(7, UpdateListWrapper)
EVT_BUTTON(9, OpenScanSettings)
EVT_CHECKBOX(10, ToggleManualUpdate)
EVT_BUTTON(11, ManualUpdateWrapper)
EVT_CHECKBOX(12, UpdateRounding)
EVT_BUTTON(13, OpenSavedMenu)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, 0, "Hex Investigator x64", wxPoint(50, 50), wxSize(550, 650))
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	selectProc = new wxButton(this, 1, "P", wxPoint(0, 0), wxSize(25, 25));
	selectProc->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectProc->SetOwnForegroundColour(wxColour(220, 220, 220));

	selectType = new wxChoice(this, 2, wxPoint(0, 0), wxSize(70, 50), wxArrayString(6, typeStrs));
	selectType->SetSelection(1);
	selectType->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectType->SetOwnForegroundColour(wxColour(220, 220, 220));

	const char* firstScans[7] =
	{
		"Equal", "Greater", "Less", "Not Equal", "Greater Or Equal", "Less Or Equal", "All"
	};

	selectScan = new wxChoice(this, 8, wxPoint(0, 0), wxSize(120, 50), wxArrayString(7, firstScans));
	selectScan->SetSelection(0);
	selectScan->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectScan->SetOwnForegroundColour(wxColour(220, 220, 220));

	isSigned = new wxCheckBox(this, wxID_ANY, "Signed");
	isSigned->SetOwnForegroundColour(wxColour(220, 220, 220));
	isSigned->SetValue(true);

	baseInputLabel = new wxStaticText(this, wxID_ANY, "Base:");
	baseInputLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	baseInput = new wxTextCtrl(this, wxID_ANY, "10", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	baseInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	baseInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	roundFloats = new wxCheckBox(this, 12, "Round floats to place:");
	roundFloats->SetOwnForegroundColour(wxColour(220, 220, 220));
	roundFloats->Disable();

	roundInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(20, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	roundInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	roundInput->SetOwnForegroundColour(wxColour(220, 220, 220));
	roundInput->SetEditable(false);

	firstScan = new wxButton(this, 5, "First Scan", wxPoint(0, 0), wxSize(75, 25));
	firstScan->SetOwnBackgroundColour(wxColour(60, 60, 60));
	firstScan->SetOwnForegroundColour(wxColour(220, 220, 220));
	firstScan->Disable();

	nextScan = new wxButton(this, 6, "Next Scan", wxPoint(0, 0), wxSize(75, 25));
	nextScan->SetOwnBackgroundColour(wxColour(60, 60, 60));
	nextScan->SetOwnForegroundColour(wxColour(220, 220, 220));
	nextScan->Disable();

	scanSettings = new ScanSettings(this);

	scanSettingsButton = new wxButton(this, 9, "Scan Settings", wxPoint(0, 0), wxSize(100, 25));
	scanSettingsButton->SetOwnBackgroundColour(wxColour(60, 60, 60));
	scanSettingsButton->SetOwnForegroundColour(wxColour(220, 220, 220));
	scanSettingsButton->Disable();

	savedMenu = new SavedMenu(procHandle);

	valueInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(9999, 25));
	valueInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	valueInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	manualUpdate = new wxCheckBox(this, 10, "Manually Update List");
	manualUpdate->SetOwnForegroundColour(wxColour(220, 220, 220));

	manualUpdateButton = new wxButton(this, 11, "Update", wxPoint(0, 0), wxSize(75, 25));
	manualUpdateButton->SetOwnBackgroundColour(wxColour(60, 60, 60));
	manualUpdateButton->SetOwnForegroundColour(wxColour(220, 220, 220));
	manualUpdateButton->Disable();

	addrList = new wxGrid(this, 3, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(wxColour(40, 40, 40));
	addrList->SetLabelTextColour(wxColour(230, 230, 230));
	addrList->SetDefaultCellBackgroundColour(wxColour(60, 60, 60));
	addrList->SetDefaultCellTextColour(wxColour(220, 220, 220));

	addrList->CreateGrid(0, 3);
	addrList->EnableGridLines(false);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->DisableDragColSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Offset");
	addrList->SetColLabelValue(2, "Value");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 80);
	addrList->SetColSize(1, 80);
	addrList->SetColSize(2, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	resultsTxt = new wxStaticText(this, wxID_ANY, "Results:");
	resultsTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	openSaved = new wxButton(this, 13, "Open Saved List", wxPoint(0, 0), wxSize(125, 25));
	openSaved->SetOwnBackgroundColour(wxColour(60, 60, 60));
	openSaved->SetOwnForegroundColour(wxColour(220, 220, 220));
	openSaved->Disable();

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(selectProc, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	row1Sizer->Add(selectType, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(selectScan, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(firstScan, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(nextScan, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(scanSettingsButton, 0, wxTOP | wxRIGHT, 10);

	row2Sizer->Add(isSigned, 0, wxLEFT | wxRIGHT, 10);
	row2Sizer->Add(baseInputLabel, 0, wxRIGHT, 5);
	row2Sizer->Add(baseInput, 0, wxRIGHT, 15);
	row2Sizer->Add(roundFloats, 0);
	row2Sizer->Add(roundInput, 0, wxRIGHT, 15);
	row2Sizer->Add(manualUpdate, 0, wxRIGHT, 5);
	row2Sizer->Add(manualUpdateButton, 0, wxRIGHT, 10);

	row3Sizer->Add(resultsTxt, 0, wxLEFT, 10);
	row3Sizer->AddStretchSpacer(1);
	row3Sizer->Add(openSaved, 0, wxRIGHT, 10);
	
	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(valueInput, 0, wxEXPAND | wxALL, 10);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(addrList, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	SetSizer(vSizer);

	updateTimer = new wxTimer(this, 7);
}

void Main::SelectProcess(wxCommandEvent& e)
{
	std::vector<std::wstring> names;
	std::vector<DWORD> ids;
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
				ids.push_back(procEntry.th32ProcessID);

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

	wxArrayString* list = new wxArrayString(count, cStrArray.data());
	int proc = wxGetSingleChoiceIndex("All Processes", "Open Process", *list, this);
	delete list;

	if (proc == -1) { return; }

	ResetScan();

	procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, ids[proc]);
	savedMenu->procHandle = procHandle;

	firstScan->Enable();
	scanSettingsButton->Enable();
	openSaved->Enable();
}

void Main::FirstScanWrapper(wxCommandEvent& e)
{
	if (scanning)
	{
		ResetScan();
		return;
	}
	
	baseInput->GetValue().ToInt(&base);
	if (base < 2 || base > 36) { base = 10; baseInput->SetValue("10"); }

	roundInput->GetValue().ToInt(&roundingPlace);
	if (roundingPlace < 0 || roundingPlace > 7)
	{
		roundingPlace = 0;
		roundingValue = 1;
		roundFloats->SetValue(false);
	}
	else { roundingValue = roundings[roundingPlace]; }
	
	int scanType = selectScan->GetSelection();
	bool noInput = (!scanning && scanType == 6) || scanType > 7;
	int type = selectType->GetSelection();
	switch (type) 
	{
	case 0:
	{
		if (!isSigned->GetValue()) 
		{
			unsigned long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->FirstScan<unsigned long long>(targetValue);
			break;
		}
		
		long long targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<long long>(targetValue);
		break;
	}
	case 1:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->FirstScan<unsigned int>(targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<int>(targetValue);
		break;
	}
	case 2:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->FirstScan<unsigned short>((unsigned short)targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<short>((short)targetValue);
		break;
	}
	case 3:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->FirstScan<unsigned char>((unsigned char)targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<char>((char)targetValue);
		break;
	}
	case 4:
	{
		double targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<float>((float)targetValue);
		break;
	}
	case 5:
	{
		double targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->FirstScan<double>(targetValue);
		break;
	}
	}

	nextScan->Enable();
	selectType->Disable();
	scanSettings->Hide();
	isSigned->Disable();

	baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
	baseInputLabel->Refresh();
	baseInput->SetEditable(false);
	baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
	baseInput->Refresh();

	firstScan->SetLabel("Reset Scan");

	if (!manualUpdate->IsChecked()) { updateTimer->Start(150); }

	scanning = true;

	selectScan->Delete(6);
	selectScan->AppendString("Increased By");
	selectScan->AppendString("Decreased By");
	selectScan->AppendString("Increased");
	selectScan->AppendString("Decreased");
	selectScan->AppendString("Changed");
	selectScan->AppendString("Unchanged");
	selectScan->SetSelection(0);
	valueInput->Show();
}

void Main::NextScanWrapper(wxCommandEvent& e)
{
	if (manualUpdate->IsChecked()) { updateTimer->Stop(); }

	int scanType = selectScan->GetSelection();
	bool noInput = (!scanning && scanType == 6) || scanType > 7;
	int type = selectType->GetSelection();
	switch (type)
	{
	case 0:
	{
		if (!isSigned->GetValue())
		{
			unsigned long long targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToULongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->NextScan<unsigned long long>(targetValue);
			break;
		}

		long long targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToLongLong(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<long long>(targetValue);
		break;
	}
	case 1:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->NextScan<unsigned int>(targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<int>(targetValue);
		break;
	}
	case 2:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->NextScan<unsigned short>((unsigned short)targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<short>((short)targetValue);
		break;
	}
	case 3:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue = 0;
			if (!noInput && !valueInput->GetValue().ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
			this->NextScan<unsigned char>((unsigned char)targetValue);
			break;
		}

		int targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<char>((char)targetValue);
		break;
	}
	case 4:
	{
		double targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<float>((float)targetValue);
		break;
	}
	case 5:
	{
		double targetValue = 0;
		if (!noInput && !valueInput->GetValue().ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Scan"); return; }
		this->NextScan<double>(targetValue);
		break;
	}
	}
}

void Main::UpdateListWrapper(wxTimerEvent& e) 
{
	int type = selectType->GetSelection();
	switch (type)
	{
	case 0:
	{
		if (isSigned->IsChecked()) { UpdateList<long long>(); }
		else { UpdateList<unsigned long long>(); }
		break;
	}
	case 1:
	{
		if (isSigned->IsChecked()) { UpdateList<int>(); }
		else { UpdateList<unsigned int>(); }
		break;
	}
	case 2:
	{
		if (isSigned->IsChecked()) { UpdateList<short>(); }
		else { UpdateList<unsigned short>(); }
		break;
	}
	case 3:
	{
		if (isSigned->IsChecked()) { UpdateList<char>(); }
		else { UpdateList<unsigned char>(); }
		break;
	}
	case 4:
	{
		UpdateList<float>();
		break;
	}
	case 5:
	{
		UpdateList<double>();
		break;
	}
	}
}

void Main::WriteValueWrapper(unsigned long long* address)
{
	wxString input = wxGetTextFromUser("New Value:", "Write Value");
	if (input == "") { return; }
	
	int type = selectType->GetSelection();
	switch (type)
	{
	case 0:
	{
		if (!isSigned->GetValue())
		{
			unsigned long long targetValue;
			if (!input.ToULongLong(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			this->WriteValue(procHandle, address, &targetValue, 8);
			break;
		}

		long long targetValue;
		if (!input.ToLongLong(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 8);
		break;
	}
	case 1:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			this->WriteValue(procHandle, address, &targetValue, 4);
			break;
		}

		int targetValue;
		if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 4);
		break;
	}
	case 2:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			this->WriteValue(procHandle, address, &targetValue, 2);
			break;
		}

		int targetValue;
		if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 2);
		break;
	}
	case 3:
	{
		if (!isSigned->GetValue())
		{
			unsigned int targetValue;
			if (!input.ToUInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
			this->WriteValue(procHandle, address, &targetValue, 1);
			break;
		}

		int targetValue;
		if (!input.ToInt(&targetValue, base)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 1);
		break;
	}
	case 4:
	{
		double targetValue;
		if (!input.ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 4);
		break;
	}
	case 5:
	{
		double targetValue;
		if (!input.ToDouble(&targetValue)) { wxMessageBox("Invalid Value", "Can't Write"); return; }
		this->WriteValue(procHandle, address, &targetValue, 8);
		break;
	}
	}
}

template <typename T> void Main::FirstScan(T targetValue)
{
	int scanType = selectScan->GetSelection();

	_MEMORY_BASIC_INFORMATION mbi;
	int results = 0;

	unsigned long long baseAddress = scanSettings->minAddress;
	unsigned long long maxAddress = scanSettings->maxAddress;

	int currentProtection = scanSettings->currentProtection;
	bool excludeZero = scanSettings->excludeZero->IsChecked();
	bool round = roundFloats->IsChecked();

	bool image = scanSettings->image;
	bool mapped = scanSettings->mapped;
	bool priv = scanSettings->priv;

	int offset = 1;
	if (scanSettings->alignMemory->IsChecked()) { offset = sizeof(T); }

	while (baseAddress < maxAddress && VirtualQueryEx(procHandle, (unsigned long long*)baseAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT && 
		   mbi.Protect == currentProtection &&
		   ((image && mbi.Type == MEM_IMAGE) || (mapped && mbi.Type == MEM_MAPPED) || (priv && mbi.Type == MEM_PRIVATE)))
		{
			unsigned char* buffer = new unsigned char[mbi.RegionSize];

			ReadProcessMemory(procHandle, mbi.BaseAddress, buffer, mbi.RegionSize, 0);

			for (int i = 0; i < mbi.RegionSize; i+=offset)
			{
				T value = *(T*)(buffer + i);

				if (round) { value = std::round(value * roundingValue) / roundingValue; }
				
				if (excludeZero && value == 0) { continue; }

				if (scanType == 6 ||
					(scanType == 0 && value == targetValue) ||
					(scanType == 1 && value > targetValue) ||
					(scanType == 2 && value < targetValue) ||
					(scanType == 3 && value != targetValue) ||
					(scanType == 4 && value >= targetValue) ||
					(scanType == 5 && value <= targetValue))
				{
					addressPool.push_back(baseAddress+i);
					oldValues.push_back(value);
					results++;
				}
			}

			delete[] buffer;
		}

		baseAddress += mbi.RegionSize;
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

	if (results == 0 || manualUpdate->IsChecked()) { return; }
	addrList->AppendRows(results);
	UpdateList<T>();
}

template <typename T> void Main::NextScan(T targetValue)
{
	std::vector<unsigned long long> newAddresses;
	std::vector<std::any> newValues;
	int results = 0;
	int size = sizeof(T);

	int scanType = selectScan->GetSelection();
	bool excludeZero = scanSettings->excludeZero->IsChecked();
	bool round = roundFloats->IsChecked();

	int len = addressPool.size();
	for (int i = 0; i < len; i++)
	{
		T value;
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], &value, size, 0);

		if (round) { value = std::round(value * roundingValue) / roundingValue; }

		if (excludeZero && value == 0) { continue; }

		T lastValue;
		if (scanType > 5) { lastValue = std::any_cast<T>(oldValues[i]); }

		if ((scanType == 0 && value == targetValue) ||
			(scanType == 1 && value > targetValue) ||
			(scanType == 2 && value < targetValue) ||
			(scanType == 3 && value != targetValue) ||
			(scanType == 4 && value >= targetValue) ||
			(scanType == 5 && value <= targetValue) ||
			(scanType == 6 && value == lastValue+targetValue) ||
			(scanType == 7 && value == lastValue-targetValue) ||
			(scanType == 8 && value > lastValue) ||
			(scanType == 9 && value < lastValue) ||
			(scanType == 10 && value != lastValue) ||
			(scanType == 11 && value == lastValue))
		{
			newAddresses.push_back(addressPool[i]);
			newValues.push_back(value);

			results++;
		}
	}

	addrList->DeleteRows(0, addrList->GetNumberRows());

	addressPool = newAddresses;
	oldValues = newValues;
	addressPool.shrink_to_fit();
	oldValues.shrink_to_fit();

	if (results > 1000)
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results) + " (1000 shown)");
		results = 1000;
	}
	else
	{
		resultsTxt->SetLabel("Results: " + std::to_string(results));
	}

	if (results == 0 || manualUpdate->IsChecked()) { return; }
	addrList->AppendRows(results);
	UpdateList<T>();
}

void Main::ResetScan() 
{
	nextScan->Disable();
	selectType->Enable();

	for (int i = 0; i < 6; i++) { selectScan->Delete(6); }
	selectScan->AppendString("All");
	selectScan->SetSelection(0);

	wxCommandEvent e;
	UpdateType(e);

	updateTimer->Stop();

	addressPool.clear();
	addressPool.shrink_to_fit();
	oldValues.clear();
	oldValues.shrink_to_fit();

	if (addrList->GetNumberRows() > 0) { addrList->DeleteRows(0, addrList->GetNumberRows()); }

	resultsTxt->SetLabel("Results: 0");

	firstScan->SetLabel("First Scan");

	scanning = false;
}

template <typename T> void Main::UpdateList() 
{
	bool isFloat = (typeid(T) == typeid(float) || typeid(T) == typeid(double));
	bool round = isFloat && roundFloats->IsChecked();
	
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }

		T value;
		ReadProcessMemory(procHandle, (unsigned long long*)addressPool[i], &value, sizeof(T), 0);

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

		std::stringstream addressToHex;
		addressToHex << std::hex << addressPool[i];

		addrList->SetCellValue(i, 0, addressToHex.str());
	}
}

void Main::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow();

	wxMenuItem* save = new wxMenuItem(0, 100, "Save");
	save->SetBackgroundColour(wxColour(60, 60, 60));
	save->SetTextColour(wxColour(220, 220, 220));
	menu.Append(save);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { savedMenu->AddAddress(addressPool[row], selectType->GetSelection(), isSigned->IsChecked(), base); }, 100);

	wxMenuItem* write = menu.Append(101, "Write Value");
	write->SetBackgroundColour(wxColour(60, 60, 60));
	write->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { WriteValueWrapper((unsigned long long*)addressPool[row]); }, 101);

	wxMenuItem* remove = menu.Append(102, "Remove");
	remove->SetBackgroundColour(wxColour(60, 60, 60));
	remove->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { RemoveRow(row); }, 102);

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
	oldValues.erase(oldValues.begin() + row);

	addrList->SetCellValue(0, 1, ""); // first row's offset is nothing
	resultsTxt->SetLabel("Results: " + std::to_string(addressPool.size()));
}

void Main::OpenScanSettings(wxCommandEvent& e)
{
	scanSettings->Show();
}

void Main::OpenSavedMenu(wxCommandEvent& e)
{
	savedMenu->Show();
}

void Main::ToggleManualUpdate(wxCommandEvent& e)
{
	if (manualUpdate->GetValue())
	{
		manualUpdateButton->Enable();
	}
	else
	{
		manualUpdateButton->Disable();
	}
}

void Main::ManualUpdateWrapper(wxCommandEvent& e) 
{
	if (!scanning) { return; }
	
	int rows = addrList->GetNumberRows();
	int add = addressPool.size() - rows;
	if (add > 0 && rows < 1000) 
	{
		if ((add + rows) > 1000) { add = 1000 - rows; }
		addrList->AppendRows(add);
	}

	updateTimer->Start(150);
}

void Main::UpdateRounding(wxCommandEvent& e)
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

void Main::UpdateType(wxCommandEvent& e) 
{
	if (selectType->GetSelection() < 4) // it's an integer
	{
		isSigned->Enable();
		baseInputLabel->SetOwnForegroundColour(wxColour(220, 220, 220));
		baseInputLabel->Refresh();
		baseInput->SetEditable(true);
		baseInput->SetLabel("10");
		baseInput->SetOwnForegroundColour(wxColour(220, 220, 220));
		baseInput->Refresh();
		base = 10;
		roundInput->SetEditable(false);
		roundInput->SetValue("");
		roundFloats->SetValue(false);
		roundFloats->Disable();
	}
	else 
	{
		isSigned->Disable();
		baseInputLabel->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInputLabel->Refresh();
		baseInput->SetEditable(false);
		baseInput->SetLabel("");
		baseInput->SetOwnForegroundColour(wxColour(100, 100, 100));
		baseInput->Refresh();
		roundFloats->Enable();
	}
}

void Main::UpdateScan(wxCommandEvent& e)
{
	if (selectScan->GetSelection() > 7 || (!scanning && selectScan->GetSelection() == 6))
	{
		valueInput->SetEditable(false);
		valueInput->SetValue("");
	}
	else
	{
		valueInput->SetEditable(true);
		valueInput->SetValue("0");
	}
} 

void Main::CloseWrapper(wxCloseEvent& e)
{
	scanSettings->Destroy();
	savedMenu->Destroy();
	this->Destroy();
}