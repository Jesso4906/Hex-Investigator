#include "SavedMenu.h"

wxBEGIN_EVENT_TABLE(SavedMenu, wxFrame)
EVT_CLOSE(CloseWrapper)
EVT_TIMER(2, UpdateListWrapper)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(3, AddAddressWrapper)
wxEND_EVENT_TABLE()

SavedMenu::SavedMenu(HANDLE procH) : wxFrame(nullptr, 0, "Saved Addresses", wxPoint(50, 50), wxSize(400, 400)) 
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	procHandle = procH;

	addAddress = new wxButton(this, 3, "Add Address", wxPoint(0, 0), wxSize(100, 25));
	addAddress->SetOwnBackgroundColour(wxColour(60, 60, 60));
	addAddress->SetOwnForegroundColour(wxColour(220, 220, 220));

	addrList = new wxGrid(this, 1, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(wxColour(40, 40, 40));
	addrList->SetLabelTextColour(wxColour(230, 230, 230));
	addrList->SetDefaultCellBackgroundColour(wxColour(60, 60, 60));
	addrList->SetDefaultCellTextColour(wxColour(220, 220, 220));

	addrList->CreateGrid(0, 5);
	addrList->EnableGridLines(false);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->DisableDragColSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Sign");
	addrList->SetColLabelValue(2, "Type");
	addrList->SetColLabelValue(3, "Base");
	addrList->SetColLabelValue(4, "Value");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 80);
	addrList->SetColSize(1, 60);
	addrList->SetColSize(2, 80);
	addrList->SetColSize(3, 40);
	addrList->SetColSize(4, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(addAddress, 0, wxCENTER | wxALL, 10);
	vSizer->Add(addrList, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);

	updateTimer = new wxTimer(this, 2);
}

void SavedMenu::AddAddress(unsigned long long address, char type, bool isSigned, char base)
{
	int row = addrList->GetNumberRows();
	addrList->AppendRows();

	std::stringstream addrToHex;
	addrToHex << std::hex << address;
	addrList->SetCellValue(row, 0, addrToHex.str());
	addrList->SetCellValue(row, 1, isSigned && type < 4 ? "Signed" : type < 4 ? "Unsigned" : "");
	addrList->SetCellValue(row, 2, typeStrs[type]);
	addrList->SetCellValue(row, 3, type < 4 ? std::to_string(base) : "");

	addresses.push_back(address);
	types.push_back(type);
	signs.push_back(isSigned);
	bases.push_back(base);

	updateTimer->Start(150);
	this->Show();
}

void SavedMenu::UpdateListWrapper(wxTimerEvent& e)
{
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }
	
		int type = types[i];
		int isSigned = signs[i];
		switch (type)
		{
		case 0:
		{
			if (isSigned) { UpdateRow<long long>(i, false); }
			else { UpdateRow<unsigned long long>(i, false); }
			break;
		}
		case 1:
		{
			if (isSigned) { UpdateRow<int>(i, false); }
			else { UpdateRow<unsigned int>(i, false); }
			break;
		}
		case 2:
		{
			if (isSigned) { UpdateRow<short>(i, false); }
			else { UpdateRow<unsigned short>(i, false); }
			break;
		}
		case 3:
		{
			if (isSigned) { UpdateRow<char>(i, false); }
			else { UpdateRow<unsigned char>(i, false); }
			break;
		}
		case 4:
		{
			UpdateRow<float>(i, true);
			break;
		}
		case 5:
		{
			UpdateRow<double>(i, true);
			break;
		}
		}
	}
}

template <typename T> void SavedMenu::UpdateRow(int row, bool isFloat)
{
	T value;
	ReadProcessMemory(procHandle, (unsigned long long*)addresses[row], &value, sizeof(T), 0);

	std::string valueStr;
	if (isFloat)
	{
		valueStr = std::to_string(value);
	}
	else
	{
		char buffer[100];
		valueStr = std::string(_itoa(value, buffer, bases[row]));
	}

	addrList->SetCellValue(row, 4, valueStr);
}

void SavedMenu::WriteValueWrapper(unsigned long long* address, char type, bool isSigned, char base)
{
	wxString input = wxGetTextFromUser("New Value:", "Write Value");
	if (input == "") { return; }

	switch (type)
	{
	case 0:
	{
		if (!isSigned)
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
		if (!isSigned)
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
		if (!isSigned)
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
		if (!isSigned)
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

void SavedMenu::RemoveRow(int row)
{
	addrList->DeleteRows(row);
	addresses.erase(addresses.begin() + row);
	types.erase(types.begin() + row);
	signs.erase(signs.begin() + row);
	bases.erase(bases.begin() + row);
}

void SavedMenu::AddAddressWrapper(wxCommandEvent& e) 
{
	wxString addressInput = wxGetTextFromUser("Enter Address", "Add Address", "0");
	if (addressInput == "") { return; }

	unsigned long long address;
	addressInput.ToULongLong(&address, 16);

	char type = wxGetSingleChoiceIndex("Type", "Enter Type", wxArrayString(6, typeStrs));
	if (type == -1) { return; }

	int base = 10;
	bool isSigned = false;

	if (type < 4) // int
	{
		base = wxGetNumberFromUser("Enter Base", "Base:", "Base", 10, 2, 36);
		if (base == -1) { return; }

		wxMessageDialog* signedMsg = new wxMessageDialog(NULL, "Set type to signed?", "Set Sign", wxYES_NO | wxCANCEL);
		int result = signedMsg->ShowModal();
		if (result == wxID_CANCEL) { return; }

		isSigned = result == wxID_YES;
	}
	
	AddAddress(address, type, isSigned, base);
}

void SavedMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow();

	wxMenuItem* write = new wxMenuItem(0, 200, "Write Value");
	write->SetBackgroundColour(wxColour(60, 60, 60));
	write->SetTextColour(wxColour(220, 220, 220));
	menu.Append(write);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { WriteValueWrapper((unsigned long long*)addresses[row], types[row], signs[row], bases[row]); }, 200);

	wxMenuItem* remove = menu.Append(201, "Remove");
	remove->SetBackgroundColour(wxColour(60, 60, 60));
	remove->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { RemoveRow(row); }, 201);

	wxMenuItem* cpyAddr = menu.Append(202, "Copy Address");
	cpyAddr->SetBackgroundColour(wxColour(60, 60, 60));
	cpyAddr->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 0)); }, 202);

	wxMenuItem* cpyOff = menu.Append(203, "Copy Type");
	cpyOff->SetBackgroundColour(wxColour(60, 60, 60));
	cpyOff->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 2)); }, 203);

	wxMenuItem* cpyVal = menu.Append(204, "Copy Value");
	cpyVal->SetBackgroundColour(wxColour(60, 60, 60));
	cpyVal->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 4)); }, 204);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x, pos.y));
	e.Skip();
}

void SavedMenu::CloseWrapper(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	updateTimer->Stop();
	this->Hide();
}