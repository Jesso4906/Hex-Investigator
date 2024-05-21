#include "ScanSettingsMenu.h"

wxBEGIN_EVENT_TABLE(ScanSettingsMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_CHECKLISTBOX(ProtectionsListID, UpdateProtection)
EVT_LISTBOX(ProtectionsListID, DeselectProtection)
EVT_CHECKLISTBOX(MemoryTypeListID, UpdateType)
EVT_LISTBOX(MemoryTypeListID, DeselectType)
EVT_TEXT_ENTER(MinAddressInputID, UpdateMinOnTextEnter)
EVT_TEXT_ENTER(MaxAddressInputID, UpdateMaxOnTextEnter)
EVT_CHOICE(SelectModuleID, UpdateSelectedModule)
EVT_CHOICE(SelectKeybindID, UpdateSelectedKeybind)
wxEND_EVENT_TABLE()

ScanSettingsMenu::ScanSettingsMenu(HANDLE hProc) : wxFrame(nullptr, MainWindowID, "Scan Settings", wxPoint(50, 50), wxSize(700, 250))
{
	procHandle = hProc;
	
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	protectionLabel = new wxStaticText(this, wxID_ANY, "Only scan regions with this protection:");
	protectionLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	protectChoice = new wxCheckListBox(this, ProtectionsListID, wxPoint(0,0), wxSize(200, 110), wxArrayString(6, protectStrs));
	protectChoice->SetOwnBackgroundColour(wxColour(60, 60, 60));
	for (int i = 0; i < 6; i++)
	{
		protectChoice->GetItem(i)->SetBackgroundColour(wxColour(60, 60, 60));
		protectChoice->GetItem(i)->SetTextColour(wxColour(220, 220, 220));
	}
	protectChoice->Check(5);

	typeLabel = new wxStaticText(this, wxID_ANY, "Only scan regions of these types:");
	typeLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	typeChoice = new wxCheckListBox(this, MemoryTypeListID, wxPoint(0, 0), wxSize(200, 60), wxArrayString(3, typeStrs));
	typeChoice->Check(0);
	typeChoice->Check(2);
	typeChoice->SetOwnBackgroundColour(wxColour(60, 60, 60));
	for (int i = 0; i < 3; i++)
	{
		typeChoice->GetItem(i)->SetBackgroundColour(wxColour(60, 60, 60));
		typeChoice->GetItem(i)->SetTextColour(wxColour(220, 220, 220));
	}

	alignMemory = new wxCheckBox(this, wxID_ANY, "Align scan with value type size");
	alignMemory->SetValue(true);
	alignMemory->SetOwnForegroundColour(wxColour(220, 220, 220));

	freezeProcess = new wxCheckBox(this, wxID_ANY, "Freeze process during scan");
	freezeProcess->SetValue(false);
	freezeProcess->SetOwnForegroundColour(wxColour(220, 220, 220));

	scanKeybindLabel = new wxStaticText(this, wxID_ANY, "First/next scan keybind:");
	scanKeybindLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	scanKeybindSelect = new wxChoice(this, SelectKeybindID, wxPoint(0, 0), wxSize(150, 25), wxArrayString(62, keyStrs));
	scanKeybindSelect->SetSelection(0);
	scanKeybindSelect->SetOwnBackgroundColour(wxColour(60, 60, 60));
	scanKeybindSelect->SetOwnForegroundColour(wxColour(220, 220, 220));

	minAddrTxt = new wxStaticText(this, wxID_ANY, "Min address:");
	minAddrTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	minAddrInput = new wxTextCtrl(this, MinAddressInputID, "0", wxPoint(0, 0), wxSize(200, 25), wxTE_PROCESS_ENTER);
	minAddrInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	minAddrInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	maxAddrTxt = new wxStaticText(this, wxID_ANY, "Max address:");
	maxAddrTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	maxAddrInput = new wxTextCtrl(this, MaxAddressInputID, "7fffffffffff", wxPoint(0, 0), wxSize(200, 25), wxTE_PROCESS_ENTER);
	maxAddrInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	maxAddrInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	moduleSelectTxt = new wxStaticText(this, wxID_ANY, "Select Module:");
	moduleSelectTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	moduleSelect = new wxChoice(this, SelectModuleID, wxPoint(0, 0), wxSize(200, 25));
	moduleSelect->SetOwnBackgroundColour(wxColour(60, 60, 60));
	moduleSelect->SetOwnForegroundColour(wxColour(220, 220, 220));

	displayModuleInfo = new wxCheckBox(this, wxID_ANY, "Display Module Info");
	displayModuleInfo->SetValue(true);
	displayModuleInfo->SetOwnForegroundColour(wxColour(220, 220, 220));

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

void ScanSettingsMenu::UpdateMinOnTextEnter(wxCommandEvent& e) { UpdateMin(); }
void ScanSettingsMenu::UpdateMaxOnTextEnter(wxCommandEvent& e) { UpdateMax(); }

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
				moduleSelect->Append(wxString(modEntry.szModule));

				ModuleBounds moduleBounds = {};
				moduleBounds.start = (uintptr_t)modEntry.modBaseAddr;
				moduleBounds.end = moduleBounds.start + (uintptr_t)modEntry.modBaseSize;

				moduleAddresses.push_back(moduleBounds);

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

bool ScanSettingsMenu::UpdateMin()
{
	uintptr_t address;
	minAddrInput->GetValue().ToULongLong(&address, 16);

	std::stringstream toHex;
	toHex << std::hex << address;
	minAddrInput->SetValue(toHex.str());

	if (address >= maxAddress) 
	{
		minAddress = 0;
		minAddrInput->SetValue("0");
		wxMessageBox("Min address must be less than max address.", "Invalid Input");
		return false;
	}

	minAddress = address;

	return true;
}
bool ScanSettingsMenu::UpdateMax()
{
	uintptr_t address;
	maxAddrInput->GetValue().ToULongLong(&address, 16);

	std::stringstream toHex;
	toHex << std::hex << address;
	maxAddrInput->SetValue(toHex.str());

	if (address <= minAddress)
	{
		maxAddress = 0x7fffffffffff;
		maxAddrInput->SetValue("7fffffffffff");
		wxMessageBox("Max address must be greater than min address.", "Invalid Input");
		return false;
	}

	maxAddress = address;
	return true;
}

void ScanSettingsMenu::OpenMenu(wxPoint position)
{
	SetPosition(position);
	Show();
	UpdateModuleAddresses();
}

void ScanSettingsMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	if (UpdateMin() && UpdateMax()) { this->Hide(); }
}