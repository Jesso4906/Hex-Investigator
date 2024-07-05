#include "SavedMenu.h"

wxBEGIN_EVENT_TABLE(SavedMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TIMER(UpdateTimerID, UpdateListOnTimer)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(AddAddressID, AddAddressButtonPress)
wxEND_EVENT_TABLE()

SavedMenu::SavedMenu(HANDLE procH, WriteMenu* wMenu) : wxFrame(nullptr, MainWindowID, "Saved Addresses", wxPoint(50, 50), wxSize(400, 400))
{
	this->SetOwnBackgroundColour(backgroundColor);

	procHandle = procH;
	writeMenu = wMenu;

	addAddress = new wxButton(this, AddAddressID, "Add Address", wxPoint(0, 0), wxSize(100, 25));
	addAddress->SetOwnBackgroundColour(foregroundColor);
	addAddress->SetOwnForegroundColour(textColor);

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(backgroundColor);
	addrList->SetLabelTextColour(textColor);
	addrList->SetDefaultCellBackgroundColour(foregroundColor);
	addrList->SetDefaultCellTextColour(textColor);

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

void SavedMenu::AddAddress(SavedEntry savedEntry)
{
	int row = addrList->GetNumberRows();
	addrList->AppendRows();

	std::stringstream addrToHex;
	addrToHex << std::hex << savedEntry.address;
	addrList->SetCellValue(row, 0, addrToHex.str());
	addrList->SetCellValue(row, 1, typeStrs[savedEntry.type]);
	addrList->SetCellValue(row, 2, std::to_string(savedEntry.base));

	savedEntries.push_back(savedEntry);
}

void SavedMenu::UpdateListOnTimer(wxTimerEvent& e)
{
	for (int i = addrList->GetFirstFullyVisibleRow(); i < addrList->GetNumberRows(); i++)
	{
		if (i == -1 || !addrList->IsVisible(i, 0, false)) { break; }
	
		ValueType type = savedEntries[i].type;
		switch (type)
		{
		case Int64:
			UpdateRow<long long>(i, false);
			break;
		case Int32:
			UpdateRow<int>(i, false);
			break;
		case Int16:
			UpdateRow<short>(i, false);
			break;
		case Int8:
			UpdateRow<char>(i, false);
			break;
		case UInt64:
			UpdateRow<unsigned long long>(i, false);
			break;
		case UInt32:
			UpdateRow<unsigned int>(i, false);
			break;
		case UInt16:
			UpdateRow<unsigned short>(i, false);
			break;
		case UInt8:
			UpdateRow<unsigned char>(i, false);
			break;
		case Float:
			UpdateRow<float>(i, true);
			break;
		case Double:
			UpdateRow<double>(i, true);
			break;
		case Bytes:
			UpdateRowByteArray(i, savedEntries[i].byteArraySize, false);
			break;
		case ASCII:
			UpdateRowByteArray(i, savedEntries[i].byteArraySize, true);
			break;
		}
	}
}

template <typename T> void SavedMenu::UpdateRow(int row, bool isFloat)
{
	T value;
	ReadProcessMemory(procHandle, (uintptr_t*)savedEntries[row].address, &value, sizeof(T), 0);

	std::string valueStr;
	if (isFloat)
	{
		valueStr = std::to_string(value);
	}
	else
	{
		char buffer[100];
		if (value < 0) { valueStr = std::string(lltoa(value, buffer, savedEntries[row].base)); }
		else { valueStr = std::string(ulltoa(value, buffer, savedEntries[row].base)); }
	}

	addrList->SetCellValue(row, 3, valueStr);
}

void SavedMenu::UpdateRowByteArray(int row, int size, bool ascii)
{
	unsigned char* value = new unsigned char[size];
	ReadProcessMemory(procHandle, (uintptr_t*)savedEntries[row].address, value, size, 0);

	std::string valueStr;

	if (ascii) 
	{
		valueStr = std::string((const char*)value).substr(0, size);
	}
	else 
	{
		for (int i = 0; i < size; i++)
		{
			std::stringstream byteToHex;
			byteToHex << std::hex << (int)value[i];
			valueStr += byteToHex.str() + ' ';
		}

		delete[] value;
	}

	addrList->SetCellValue(row, 3, valueStr);
}

void SavedMenu::WriteValueHandler(wxString input, uintptr_t* address, ValueType type, char base)
{
	if (input == "") { return; }

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

void SavedMenu::RemoveRow(int row)
{
	addrList->DeleteRows(row);
	savedEntries.erase(savedEntries.begin() + row);
}

void SavedMenu::AddAddressButtonPress(wxCommandEvent& e) 
{
	wxString addressInput = wxGetTextFromUser("Enter Address", "Add Address", "0");
	if (addressInput == "") { return; }

	uintptr_t address;
	addressInput.ToULongLong(&address, 16);

	ValueType type = (ValueType)wxGetSingleChoiceIndex("Type", "Enter Type", wxArrayString(12, typeStrs));
	if (type == -1) { return; }

	int base = 10;

	int byteArraySize = 0; // for if its a byte array

	if (type < Float) // it is an int
	{
		base = wxGetNumberFromUser("Enter Base", "Base:", "Base", 10, 2, 36);
		if (base == -1) { return; }
	}
	else if (type > Double) //byte array or string
	{
		base = 16;
		byteArraySize = wxGetNumberFromUser("Enter Size", "Size:", "Size", 1, 1, 1000);
	}

	SavedEntry savedEntry;
	savedEntry.address = address;
	savedEntry.type = type;
	savedEntry.base = base;
	savedEntry.byteArraySize = byteArraySize;
	
	AddAddress(savedEntry);
}

void SavedMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right clicked on
	wxArrayInt selectedRows = addrList->GetSelectedRows(); // all rows also selected

	if (savedEntries[row].type < Bytes) // cant write if its a byte array
	{
		wxMenuItem* write = new wxMenuItem(0, 200, "Write Value");
		write->SetBackgroundColour(foregroundColor);
		write->SetTextColour(textColor);
		menu.Append(write);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
			{ 
				wxString input = wxGetTextFromUser("New Value:", "Write Value");

				if (selectedRows.IsEmpty()) { WriteValueHandler(input, (uintptr_t*)savedEntries[row].address, savedEntries[row].type, savedEntries[row].base); }

				for (int i = 0; i < selectedRows.GetCount(); i++) 
				{
					int currentRow = selectedRows.Item(i);
					WriteValueHandler(input, (uintptr_t*)savedEntries[currentRow].address, savedEntries[currentRow].type, savedEntries[currentRow].base);
				}
			}, 200);

		wxMenuItem* setWriteOp = new wxMenuItem(0, 205, "Set Write Operation");
		setWriteOp->SetBackgroundColour(foregroundColor);
		setWriteOp->SetTextColour(textColor);
		menu.Append(setWriteOp);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void
			{
				WriteMenu::WriteEntry writeEntry;
				writeEntry.address = savedEntries[row].address;
				writeEntry.type = savedEntries[row].type;
				
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
					writeEntry.address = savedEntries[currentRow].address;
					writeMenu->AddWriteEntry(writeEntry);
				}
			}, 205);
	}

	wxMenuItem* remove = new wxMenuItem(0, 201, "Remove");
	remove->SetBackgroundColour(foregroundColor);
	remove->SetTextColour(textColor);
	menu.Append(remove);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void 
		{ 
			if (selectedRows.IsEmpty()) { RemoveRow(row); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				RemoveRow(selectedRows.Item(i)-i);
			}
		}, 201);

	wxMenuItem* cpyAddr = menu.Append(202, "Copy Address");
	cpyAddr->SetBackgroundColour(foregroundColor);
	cpyAddr->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 0)); }, 202);

	wxMenuItem* cpyOff = menu.Append(203, "Copy Type");
	cpyOff->SetBackgroundColour(foregroundColor);
	cpyOff->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 2)); }, 203);

	wxMenuItem* cpyVal = menu.Append(204, "Copy Value");
	cpyVal->SetBackgroundColour(foregroundColor);
	cpyVal->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 4)); }, 204);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x+10, pos.y+45));
	e.Skip();
}

void SavedMenu::OpenMenu(wxPoint position)
{
	updateTimer->Start(updateRate);
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void SavedMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	updateTimer->Stop();
	Hide();
}