#include "ScanSettingsMenu.h"

wxBEGIN_EVENT_TABLE(ScanSettingsMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_CHECKLISTBOX(ProtectionsListID, UpdateProtection)
EVT_LISTBOX(ProtectionsListID, DeselectProtection)
EVT_CHECKLISTBOX(MemoryTypeListID, UpdateType)
EVT_LISTBOX(MemoryTypeListID, DeselectType)
EVT_CHOICE(SelectModuleID, UpdateSelectedModule)
EVT_CHOICE(SelectKeybindID, UpdateSelectedKeybind)
wxEND_EVENT_TABLE()

ScanSettingsMenu::ScanSettingsMenu(HANDLE hProc) : wxFrame(nullptr, MainWindowID, "Scan Settings", wxPoint(50, 50), wxSize(700, 250))
{
	procHandle = hProc;
	
	this->SetOwnBackgroundColour(backgroundColor);

	protectionLabel = new wxStaticText(this, wxID_ANY, "Only scan regions with this protection:");
	protectionLabel->SetOwnForegroundColour(textColor);

	protectChoice = new wxCheckListBox(this, ProtectionsListID, wxPoint(0,0), wxSize(200, 110), wxArrayString(6, protectStrs));
	protectChoice->SetOwnBackgroundColour(foregroundColor);
	for (int i = 0; i < 6; i++)
	{
		protectChoice->GetItem(i)->SetBackgroundColour(foregroundColor);
		protectChoice->GetItem(i)->SetTextColour(textColor);
	}
	protectChoice->Check(5);

	typeLabel = new wxStaticText(this, wxID_ANY, "Only scan regions of these types:");
	typeLabel->SetOwnForegroundColour(textColor);

	typeChoice = new wxCheckListBox(this, MemoryTypeListID, wxPoint(0, 0), wxSize(200, 60), wxArrayString(3, typeStrs));
	typeChoice->Check(0);
	typeChoice->Check(2);
	typeChoice->SetOwnBackgroundColour(foregroundColor);
	for (int i = 0; i < 3; i++)
	{
		typeChoice->GetItem(i)->SetBackgroundColour(foregroundColor);
		typeChoice->GetItem(i)->SetTextColour(textColor);
	}

	alignMemory = new wxCheckBox(this, wxID_ANY, "Align scan with value type size");
	alignMemory->SetValue(true);
	alignMemory->SetOwnForegroundColour(textColor);

	freezeProcess = new wxCheckBox(this, wxID_ANY, "Freeze process during scan");
	freezeProcess->SetValue(false);
	freezeProcess->SetOwnForegroundColour(textColor);

	scanKeybindLabel = new wxStaticText(this, wxID_ANY, "First/next scan keybind:");
	scanKeybindLabel->SetOwnForegroundColour(textColor);

	scanKeybindSelect = new wxChoice(this, SelectKeybindID, wxPoint(0, 0), wxSize(150, 25), wxArrayString(62, keyStrs));
	scanKeybindSelect->SetSelection(0);
	scanKeybindSelect->SetOwnBackgroundColour(foregroundColor);
	scanKeybindSelect->SetOwnForegroundColour(textColor);

	minAddrTxt = new wxStaticText(this, wxID_ANY, "Min address:");
	minAddrTxt->SetOwnForegroundColour(textColor);

	minAddrInput = new wxTextCtrl(this, MinAddressInputID, "0", wxPoint(0, 0), wxSize(200, 25), wxTE_PROCESS_ENTER);
	minAddrInput->SetOwnBackgroundColour(foregroundColor);
	minAddrInput->SetOwnForegroundColour(textColor);

	maxAddrTxt = new wxStaticText(this, wxID_ANY, "Max address:");
	maxAddrTxt->SetOwnForegroundColour(textColor);

	maxAddrInput = new wxTextCtrl(this, MaxAddressInputID, "7fffffffffff", wxPoint(0, 0), wxSize(200, 25), wxTE_PROCESS_ENTER);
	maxAddrInput->SetOwnBackgroundColour(foregroundColor);
	maxAddrInput->SetOwnForegroundColour(textColor);

	moduleSelectTxt = new wxStaticText(this, wxID_ANY, "Select Module:");
	moduleSelectTxt->SetOwnForegroundColour(textColor);

	moduleSelect = new wxChoice(this, SelectModuleID, wxPoint(0, 0), wxSize(200, 25));
	moduleSelect->SetOwnBackgroundColour(foregroundColor);
	moduleSelect->SetOwnForegroundColour(textColor);

	displayModuleInfo = new wxCheckBox(this, wxID_ANY, "Display Module Info");
	displayModuleInfo->SetValue(true);
	displayModuleInfo->SetOwnForegroundColour(textColor);

	column1Sizer = new wxBoxSizer(wxVERTICAL);
	column2Sizer = new wxBoxSizer(wxVERTICAL);
	column3Sizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	column1Sizer->Add(protectionLabel, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column1Sizer->Add(protectChoice, 0, wxLEFT | wxRIGHT, 10);

	column2Sizer->Add(typeLabel, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column2Sizer->Add(typeChoice, 0, wxLEFT | wxRIGHT, 10);
	column2Sizer->Add(alignMemory, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column2Sizer->Add(freezeProcess, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column2Sizer->Add(scanKeybindLabel, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column2Sizer->Add(scanKeybindSelect, 0, wxLEFT | wxTOP | wxRIGHT, 10);

	column3Sizer->Add(minAddrTxt, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column3Sizer->Add(minAddrInput, 0, wxLEFT | wxRIGHT, 10);
	column3Sizer->Add(maxAddrTxt, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column3Sizer->Add(maxAddrInput, 0, wxLEFT | wxRIGHT, 10);
	column3Sizer->Add(moduleSelectTxt, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column3Sizer->Add(moduleSelect, 0, wxLEFT | wxRIGHT, 10);
	column3Sizer->Add(displayModuleInfo, 0, wxLEFT | wxTOP | wxRIGHT, 10);

	hSizer->Add(column1Sizer);
	hSizer->Add(column2Sizer);
	hSizer->Add(column3Sizer);

	SetSizer(hSizer);
}

void ScanSettingsMenu::UpdateProtection(wxCommandEvent& e)
{
	int selection = e.GetInt();

	for (int i = 0; i < protectChoice->GetCount(); i++)
	{
		protectChoice->Check(i, i == selection);
	}

	currentProtection = protections[selection];
}

void ScanSettingsMenu::DeselectProtection(wxCommandEvent& e)
{
	protectChoice->DeselectAll();
}

void ScanSettingsMenu::UpdateType(wxCommandEvent& e)
{
	image = typeChoice->IsChecked(0);
	mapped = typeChoice->IsChecked(1);
	priv = typeChoice->IsChecked(2);
}

void ScanSettingsMenu::DeselectType(wxCommandEvent& e)
{
	typeChoice->DeselectAll();
}

void ScanSettingsMenu::UpdateSelectedModule(wxCommandEvent& e)
{
	uintptr_t min;
	uintptr_t max;

	int selection = e.GetSelection();

	if (selection == 0) // none option
	{
		min = 0;
		max = 0x7fffffffffff;
	}
	else 
	{
		min = moduleAddresses[selection-1].start;
		max = moduleAddresses[selection-1].end;
	}
	
	std::stringstream toHexMin;
	toHexMin << std::hex << min;
	minAddrInput->SetValue(toHexMin.str());
	minAddress = min;

	std::stringstream toHexMax;
	toHexMax << std::hex << max;
	maxAddrInput->SetValue(toHexMax.str());
	maxAddress = max;
}

void ScanSettingsMenu::UpdateModuleAddresses()
{
	moduleAddresses.clear();
	moduleAddresses.shrink_to_fit();

	int previousSelection = moduleSelect->GetSelection();
	previousSelection = previousSelection == -1 ? 0 : previousSelection;

	moduleSelect->Clear();
	moduleSelect->Append("None");
	
	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(procHandle));

	if (modSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(modSnap, &modEntry))
		{
			do
			{
				ModuleBounds moduleBounds = {};
				moduleBounds.start = (uintptr_t)modEntry.modBaseAddr;
				moduleBounds.end = moduleBounds.start + (uintptr_t)modEntry.modBaseSize;
				
				int count = moduleSelect->GetCount();
				if (count == 1)
				{
					moduleSelect->Append(wxString(modEntry.szModule));
					moduleAddresses.push_back(moduleBounds);
				}
				else 
				{
					for (int i = 1; i < count; i++) 
					{
						wxString str1 = moduleSelect->GetString(i).Lower();
						wxString str2 = wxString(modEntry.szModule);
						if (str1.compare(str2.Lower()) >= 0)
						{
							moduleSelect->Insert(str2, i);
							moduleAddresses.insert(moduleAddresses.begin() + (i - 1), moduleBounds);
							break;
						}
						else if (i == count - 1) 
						{ 
							moduleSelect->Append(wxString(modEntry.szModule));
							moduleAddresses.push_back(moduleBounds);
						}
					}
				}

			} while (Module32Next(modSnap, &modEntry));
		}
	}
	CloseHandle(modSnap);

	moduleSelect->SetSelection(previousSelection);
}

void ScanSettingsMenu::UpdateSelectedKeybind(wxCommandEvent& e)
{
	scanKeybind = keyCodes[e.GetSelection()];
}

bool ScanSettingsMenu::UpdateMinAndMax()
{
	uintptr_t newMinAddress;
	minAddrInput->GetValue().ToULongLong(&newMinAddress, 16);

	std::stringstream minToHex;
	minToHex << std::hex << newMinAddress;
	minAddrInput->SetValue(minToHex.str());

	minAddress = newMinAddress;

	uintptr_t newMaxAddress;
	maxAddrInput->GetValue().ToULongLong(&newMaxAddress, 16);

	std::stringstream maxToHex;
	maxToHex << std::hex << newMaxAddress;
	maxAddrInput->SetValue(maxToHex.str());

	maxAddress = newMaxAddress;

	if (minAddress >= maxAddress)
	{
		minAddress = 0;
		minAddrInput->SetValue("0");
		maxAddress = 0x7fffffffffff;
		maxAddrInput->SetValue("7fffffffffff");
		wxMessageBox("Min address must be less than max address.", "Invalid Input");
		return false;
	}

	return true;
}

void ScanSettingsMenu::OpenMenu(wxPoint position)
{
	SetPosition(position);
	Show();
	UpdateModuleAddresses();
	Raise();
}

void ScanSettingsMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	if (UpdateMinAndMax()) { this->Hide(); }
}