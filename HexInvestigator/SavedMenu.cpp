#include "SavedMenu.h"

wxBEGIN_EVENT_TABLE(SavedMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TIMER(UpdateTimerID, UpdateListOnTimer)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(AddAddressID, AddAddressButtonPress)
wxEND_EVENT_TABLE()

SavedMenu::SavedMenu(HANDLE procH) : wxFrame(nullptr, MainWindowID, "Saved Addresses", wxPoint(50, 50), wxSize(400, 400))
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	procHandle = procH;

	addAddress = new wxButton(this, AddAddressID, "Add Address", wxPoint(0, 0), wxSize(100, 25));
	addAddress->SetOwnBackgroundColour(wxColour(60, 60, 60));
	addAddress->SetOwnForegroundColour(wxColour(220, 220, 220));

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(wxColour(40, 40, 40));
	addrList->SetLabelTextColour(wxColour(230, 230, 230));
	addrList->SetDefaultCellBackgroundColour(wxColour(60, 60, 60));
	addrList->SetDefaultCellTextColour(wxColour(220, 220, 220));

	addrList->CreateGrid(0, 4);
	addrList->EnableGridLines(false);
	addrList->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->DisableDragColSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Type");
	addrList->SetColLabelValue(2, "Base");
	addrList->SetColLabelValue(3, "Value");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 80);
	addrList->SetColSize(1, 60);
	addrList->SetColSize(2, 80);
	addrList->SetColSize(3, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(addAddress, 0, wxCENTER | wxALL, 10);
	vSizer->Add(addrList, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);

	updateTimer = new wxTimer(this, UpdateTimerID);
}

void SavedMenu::AddAddress(uintptr_t address, char type, char base, int size=0)
{
	int row = addrList->GetNumberRows();
	addrList->AppendRows();

	std::stringstream addrToHex;
	addrToHex << std::hex << address;
	addrList->SetCellValue(row, 0, addrToHex.str());
	addrList->SetCellValue(row, 1, typeStrs[type]);
	addrList->SetCellValue(row, 2, std::to_string(base));

	addresses.push_back(address);
	types.push_back(type);
	bases.push_back(base);
	sizes.push_back(size);
}

void SavedMenu::UpdateListOnTimer(wxTimerEvent& e)
{
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }
	
		int type = types[i];
		switch (type)
		{
		case 0:
			UpdateRow<long long>(i, false);
			break;
		case 1:
			UpdateRow<int>(i, false);
			break;
		case 2:
			UpdateRow<short>(i, false);
			break;
		case 3:
			UpdateRow<char>(i, false);
			break;
		case 4:
			UpdateRow<unsigned long long>(i, false);
			break;
		case 5:
			UpdateRow<unsigned int>(i, false);
			break;
		case 6:
			UpdateRow<unsigned short>(i, false);
			break;
		case 7:
			UpdateRow<unsigned char>(i, false);
			break;
		case 8:
			UpdateRow<float>(i, true);
			break;
		case 9:
			UpdateRow<double>(i, true);
			break;
		case 10:
			UpdateRowByteArray(i, sizes[i]);
			break;
		}
	}
}

template <typename T> void SavedMenu::UpdateRow(int row, bool isFloat)
{
	T value;
	ReadProcessMemory(procHandle, (uintptr_t*)addresses[row], &value, sizeof(T), 0);

	std::string valueStr;
	if (isFloat)
	{
		valueStr = std::to_string(value);
	}
	else
	{
		char buffer[100];
		if (value < 0) { valueStr = std::string(lltoa(value, buffer, bases[row])); }
		else { valueStr = std::string(ulltoa(value, buffer, bases[row])); }
	}

	addrList->SetCellValue(row, 3, valueStr);
}

void SavedMenu::UpdateRowByteArray(int row, int size)
{
	unsigned char* value = new unsigned char[size];
	ReadProcessMemory(procHandle, (uintptr_t*)addresses[row], value, size, 0);

	std::string valueStr;
	for (int i = 0; i < size; i++)
	{
		std::stringstream byteToHex;
		byteToHex << std::hex << (int)value[i];
		valueStr += byteToHex.str() + ' ';
	}

	delete[] value;

	addrList->SetCellValue(row, 3, valueStr);
}

void SavedMenu::WriteValueHandler(wxString input, uintptr_t* address, char type, char base)
{
	if (input == "") { return; }

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
			double doubleVal = 0;
			if (!input.ToDouble(&doubleVal)) { wxMessageBox("Invalid Value", "Can't Write"); return; }

			float targetValue = (float)doubleVal;
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

void SavedMenu::RemoveRow(int row)
{
	addrList->DeleteRows(row);
	addresses.erase(addresses.begin() + row);
	types.erase(types.begin() + row);
	bases.erase(bases.begin() + row);
}

void SavedMenu::AddAddressButtonPress(wxCommandEvent& e) 
{
	wxString addressInput = wxGetTextFromUser("Enter Address", "Add Address", "0");
	if (addressInput == "") { return; }

	uintptr_t address;
	addressInput.ToULongLong(&address, 16);

	char type = wxGetSingleChoiceIndex("Type", "Enter Type", wxArrayString(11, typeStrs));
	if (type == -1) { return; }

	int base = 10;

	int size = 0; // for if its a byte array

	if (type < 8) // int
	{
		base = wxGetNumberFromUser("Enter Base", "Base:", "Base", 10, 2, 36);
		if (base == -1) { return; }
	}
	else if (type == 10) //byte array
	{
		base = 16;
		size = wxGetNumberFromUser("Enter Size", "Size:", "Size", 1, 1, 1000);
	}
	
	AddAddress(address, type, base, size);
}

void SavedMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right clicked on
	wxArrayInt selectedRows = addrList->GetSelectedRows(); // all rows also selected

	if (types[row] != 10) // cant write if its a byte array
	{
		wxMenuItem* write = new wxMenuItem(0, 200, "Write Value");
		write->SetBackgroundColour(wxColour(60, 60, 60));
		write->SetTextColour(wxColour(220, 220, 220));
		menu.Append(write);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
			{ 
				wxString input = wxGetTextFromUser("New Value:", "Write Value");

				if (selectedRows.IsEmpty()) { WriteValueHandler(input, (uintptr_t*)addresses[row], types[row], bases[row]); }

				for (int i = 0; i < selectedRows.GetCount(); i++) 
				{
					int currentRow = selectedRows.Item(i);
					WriteValueHandler(input, (uintptr_t*)addresses[currentRow], types[currentRow], bases[currentRow]);
				}
			}, 200);
	}

	wxMenuItem* remove = menu.Append(201, "Remove");
	remove->SetBackgroundColour(wxColour(60, 60, 60));
	remove->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (selectedRows.IsEmpty()) { RemoveRow(row); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				RemoveRow(selectedRows.Item(i)-i);
			}
		}, 201);

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
	PopupMenu(&menu, wxPoint(pos.x+10, pos.y+45));
	e.Skip();
}

void SavedMenu::OpenMenu(wxPoint position)
{
	updateTimer->Start(updateRate);
	SetPosition(position);
	Show();
}

void SavedMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	updateTimer->Stop();
	Hide();
}