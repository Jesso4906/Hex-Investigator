#include "WriteMenu.h"

wxBEGIN_EVENT_TABLE(WriteMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TIMER(WriteTimerID, WriteOnTimer)
EVT_BUTTON(SetWriteOperationID, AddAddressButtonPress)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
wxEND_EVENT_TABLE()

WriteMenu::WriteMenu(HANDLE procH) : wxFrame(nullptr, MainWindowID, "Write Menu", wxPoint(50, 50), wxSize(400, 400))
{
	this->SetOwnBackgroundColour(backgroundColor);

	procHandle = procH;

	setWriteOperation = new wxButton(this, SetWriteOperationID, "Set Write Operation", wxPoint(0, 0), wxSize(150, 25));
	setWriteOperation->SetOwnBackgroundColour(foregroundColor);
	setWriteOperation->SetOwnForegroundColour(textColor);

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(backgroundColor);
	addrList->SetLabelTextColour(textColor);
	addrList->SetDefaultCellBackgroundColour(foregroundColor);
	addrList->SetDefaultCellTextColour(textColor);

	addrList->CreateGrid(0, 5);
	addrList->EnableGridLines(false);
	addrList->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->DisableDragColSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Type");
	addrList->SetColLabelValue(2, "Operation");
	addrList->SetColLabelValue(3, "Value Used");
	addrList->SetColLabelValue(4, "Frequency");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 80);
	addrList->SetColSize(1, 60);
	addrList->SetColSize(2, 80);
	addrList->SetColSize(3, 80);
	addrList->SetColSize(4, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(setWriteOperation, 0, wxCENTER | wxALL, 10);
	vSizer->Add(addrList, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);

	writeTimer = new wxTimer(this, WriteTimerID);
}

void WriteMenu::AddWriteEntry(WriteEntry entry)
{
	int row = addrList->GetNumberRows();
	addrList->AppendRows();

	std::stringstream addrToHex;
	addrToHex << std::hex << entry.address;

	addrList->SetCellValue(row, 0, addrToHex.str());
	addrList->SetCellValue(row, 1, typeStrs[entry.type]);
	addrList->SetCellValue(row, 2, operationStrs[entry.operation]);
	addrList->SetCellValue(row, 3, entry.valueStr);
	addrList->SetCellValue(row, 4, std::to_string(entry.frequency));

	writeEntries.push_back(entry);

	lastTime = std::chrono::high_resolution_clock::now();
	writeTimer->Start(1);
}

void WriteMenu::WriteOnTimer(wxTimerEvent& e)
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
	lastTime = currentTime;
	
	for (int i = 0; i < writeEntries.size(); i++) 
	{
		if (writeEntries[i].timer >= writeEntries[i].frequency) 
		{
			WriteOperationHandler(writeEntries[i]);
			
			writeEntries[i].timer = 0;
		}
		else 
		{ 
			writeEntries[i].timer += deltaTime.count(); 
		}
	}
}

void WriteMenu::WriteOperationHandler(WriteEntry writeEntry)
{
	switch (writeEntry.type) 
	{
	case Int64:
		ExecuteWriteOperation<long long>(writeEntry);
		break;
	case Int32:
		ExecuteWriteOperation<int>(writeEntry);
		break;
	case Int16:
		ExecuteWriteOperation<short>(writeEntry);
		break;
	case Int8:
		ExecuteWriteOperation<char>(writeEntry);
		break;
	case UInt64:
		ExecuteWriteOperation<unsigned long long>(writeEntry);
		break;
	case UInt32:
		ExecuteWriteOperation<unsigned int>(writeEntry);
		break;
	case UInt16:
		ExecuteWriteOperation<unsigned short>(writeEntry);
		break;
	case UInt8:
		ExecuteWriteOperation<unsigned char>(writeEntry);
		break;
	case Float:
		ExecuteWriteOperation<float>(writeEntry);
		break;
	case Double:
		ExecuteWriteOperation<double>(writeEntry);
		break;
	}
}

template <typename T> void WriteMenu::ExecuteWriteOperation(WriteEntry writeEntry)
{
	if (writeEntry.operation == Set)
	{
		T newValue = (T)writeEntry.value;
		WriteProcessMemory(procHandle, (void*)writeEntry.address, &newValue, sizeof(T), nullptr);
		return;
	}

	T oldVal;
	ReadProcessMemory(procHandle, (void*)writeEntry.address, &oldVal, sizeof(T), nullptr);

	T newValue;
	switch (writeEntry.operation)
	{
	case Add:
		newValue = oldVal + writeEntry.value;
		break;
	case Subtract:
		newValue = oldVal - writeEntry.value;
		break;
	case Multiply:
		newValue = oldVal * writeEntry.value;
		break;
	case Divide:
		newValue = oldVal / writeEntry.value;
		break;
	}

	WriteProcessMemory(procHandle, (void*)writeEntry.address, &newValue, sizeof(T), nullptr);
}

void WriteMenu::RemoveRow(int row)
{
	addrList->DeleteRows(row);
	writeEntries.erase(writeEntries.begin() + row);

	if (writeEntries.size() == 0) { writeTimer->Stop(); }
}

void WriteMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on
	wxArrayInt selectedRows = addrList->GetSelectedRows(); // all rows also selected

	wxMenuItem* remove = new wxMenuItem(0, 201, "Remove");
	remove->SetBackgroundColour(foregroundColor);
	remove->SetTextColour(textColor);
	menu.Append(remove);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void
		{
			if (selectedRows.IsEmpty()) { RemoveRow(row); }

			for (int i = 0; i < selectedRows.GetCount(); i++)
			{
				RemoveRow(selectedRows.Item(i) - i);
			}
		}, 201);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x + 10, pos.y + 45));
	e.Skip();
}

void WriteMenu::AddAddressButtonPress(wxCommandEvent& e)
{
	WriteEntry entry;
	
	wxString addressInput = wxGetTextFromUser("Enter the address to write to", "Address", "0");
	if (addressInput == "") { return; }

	uintptr_t address;
	addressInput.ToULongLong(&address, 16);
	entry.address = address;

	entry.type = (ValueType)wxGetSingleChoiceIndex("Chose the value type", "Type", wxArrayString(10, typeStrs));
	if (entry.type == -1) { return; }

	entry.operation = (Operation)wxGetSingleChoiceIndex("Chose the operation to execute", "Operation", wxArrayString(5, operationStrs));
	if (entry.operation == -1) { return; }

	wxString valueInput = wxGetTextFromUser("Enter the value to use in the operation", "Value", "1");
	if (!valueInput.ToDouble(&entry.value)) { return; }
	entry.valueStr = valueInput;

	entry.frequency = wxGetNumberFromUser("The operation will be executed once every this many miliseconds", "Frequency:", "Frequency", 0, 0, 100000);
	if (entry.frequency == -1) { return; }

	AddWriteEntry(entry);
}

void WriteMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void WriteMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}