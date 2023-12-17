#include "ScanSettingsMenu.h"

wxBEGIN_EVENT_TABLE(ScanSettingsMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_CHECKLISTBOX(ProtectionsListID, UpdateProtection)
EVT_LISTBOX(ProtectionsListID, DeselectProtection)
EVT_CHECKLISTBOX(MemoryTypeListID, UpdateType)
EVT_LISTBOX(MemoryTypeListID, DeselectType)
EVT_TEXT_ENTER(MinAddressInputID, UpdateMinOnTextEnter)
EVT_TEXT_ENTER(MaxAddressInputID, UpdateMaxOnTextEnter)
wxEND_EVENT_TABLE()

ScanSettingsMenu::ScanSettingsMenu(wxWindow* parent) : wxFrame(parent, MainWindowID, "Scan Settings", wxPoint(50, 50), wxSize(780, 175))
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	protectionLabel = new wxStaticText(this, wxID_ANY, "Only scan regions with this protection:");
	protectionLabel->SetOwnForegroundColour(wxColour(220, 220, 220));

	protectChoice = new wxCheckListBox(this, ProtectionsListID, wxPoint(0,0), wxSize(200, 95), wxArrayString(5, protectStrs));
	protectChoice->SetOwnBackgroundColour(wxColour(60, 60, 60));
	for (int i = 0; i < 5; i++) 
	{
		protectChoice->GetItem(i)->SetBackgroundColour(wxColour(60, 60, 60));
		protectChoice->GetItem(i)->SetTextColour(wxColour(220, 220, 220));
	}
	protectChoice->Check(4);

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

	alignMemory = new wxCheckBox(this, wxID_ANY, "Align memory with type size");
	alignMemory->SetValue(true);
	alignMemory->SetOwnForegroundColour(wxColour(220, 220, 220));

	minAddrTxt = new wxStaticText(this, wxID_ANY, "Min address:");
	minAddrTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	minAddrInput = new wxTextCtrl(this, MinAddressInputID, "0", wxPoint(0, 0), wxSize(100, 25), wxTE_PROCESS_ENTER);
	minAddrInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	minAddrInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	maxAddrTxt = new wxStaticText(this, wxID_ANY, "Max address:");
	maxAddrTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	maxAddrInput = new wxTextCtrl(this, MaxAddressInputID, "7fffffffffff", wxPoint(0, 0), wxSize(100, 25), wxTE_PROCESS_ENTER);
	maxAddrInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	maxAddrInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	column1Sizer = new wxBoxSizer(wxVERTICAL);
	column2Sizer = new wxBoxSizer(wxVERTICAL);
	column3Sizer = new wxBoxSizer(wxVERTICAL);
	column4Sizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);

	column1Sizer->Add(protectionLabel, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column1Sizer->Add(protectChoice, 0, wxLEFT | wxRIGHT, 10);

	column2Sizer->Add(typeLabel, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column2Sizer->Add(typeChoice, 0, wxLEFT | wxRIGHT, 10);

	column3Sizer->Add(minAddrTxt, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column3Sizer->Add(minAddrInput, 0, wxLEFT | wxRIGHT, 10);
	column3Sizer->Add(maxAddrTxt, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	column3Sizer->Add(maxAddrInput, 0, wxLEFT | wxRIGHT, 10);

	column4Sizer->Add(alignMemory, 0, wxLEFT | wxTOP | wxRIGHT, 10);

	hSizer->Add(column1Sizer);
	hSizer->Add(column2Sizer);
	hSizer->Add(column3Sizer);
	hSizer->Add(column4Sizer);

	SetSizer(hSizer);
}

void ScanSettingsMenu::UpdateProtection(wxCommandEvent& e)
{
	int selection = e.GetInt();

	if (!protectChoice->IsChecked(selection)) 
	{ 
		protectChoice->Check(selection);
		return;
	}

	for (int i = 0; i < 7; i++) 
	{
		if (i != selection)
		{
			protectChoice->Check(i, false);
		}
	}

	currentProtection = protections[selection];
}

void ScanSettingsMenu::DeselectProtection(wxCommandEvent& e)
{
	protectChoice->DeselectAll();
}

void ScanSettingsMenu::UpdateType(wxCommandEvent& e)
{
	int selection = e.GetInt();

	if (selection == 0)
	{
		image = typeChoice->IsChecked(0);
		return;
	}
	else if (selection == 1) 
	{
		mapped = typeChoice->IsChecked(1);
		return;
	}

	priv = typeChoice->IsChecked(2);
}

void ScanSettingsMenu::DeselectType(wxCommandEvent& e)
{
	typeChoice->DeselectAll();
}

void ScanSettingsMenu::UpdateMinOnTextEnter(wxCommandEvent& e) { UpdateMin(); }
void ScanSettingsMenu::UpdateMaxOnTextEnter(wxCommandEvent& e) { UpdateMax(); }

bool ScanSettingsMenu::UpdateMin()
{
	unsigned long long address;
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
	unsigned long long address;
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
}

void ScanSettingsMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	if (UpdateMin() && UpdateMax()) { this->Hide(); }
}