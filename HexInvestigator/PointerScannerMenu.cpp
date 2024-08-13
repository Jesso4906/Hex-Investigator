#include "PointerScannerMenu.h"

wxBEGIN_EVENT_TABLE(PointerScannerMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(ScanButtonID, ScanButtonPress)
wxEND_EVENT_TABLE()

PointerScannerMenu::PointerScannerMenu(HANDLE procH) : wxFrame(nullptr, MainWindowID, "Pointer Scanner", wxPoint(50, 50), wxSize(600, 600)) 
{
	this->SetOwnBackgroundColour(backgroundColor);

	procHandle = procH;

	addressInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(300, 25));
	addressInput->SetOwnBackgroundColour(foregroundColor);
	addressInput->SetOwnForegroundColour(textColor);
	addressInput->SetToolTip("Address to scan for pointers to");

	levelsInputLabel = new wxStaticText(this, wxID_ANY, "Levels:");
	levelsInputLabel->SetOwnForegroundColour(textColor);

	levelsInput = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(0, 0), wxSize(40, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	levelsInput->SetOwnBackgroundColour(foregroundColor);
	levelsInput->SetOwnForegroundColour(textColor);

	maxOffsetInputLabel = new wxStaticText(this, wxID_ANY, "Max Offset:");
	maxOffsetInputLabel->SetOwnForegroundColour(textColor);

	maxOffsetInput = new wxTextCtrl(this, wxID_ANY, "100", wxPoint(0, 0), wxSize(40, 20), wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
	maxOffsetInput->SetOwnBackgroundColour(foregroundColor);
	maxOffsetInput->SetOwnForegroundColour(textColor);

	resultsTxt = new wxStaticText(this, wxID_ANY, "Results:");
	resultsTxt->SetOwnForegroundColour(textColor);

	scanButton = new wxButton(this, ScanButtonID, "Scan", wxPoint(0, 0), wxSize(100, 25));
	scanButton->SetOwnBackgroundColour(foregroundColor);
	scanButton->SetOwnForegroundColour(textColor);

	pointerGrid = new wxGrid(this, PointerGridID, wxPoint(0, 0), wxSize(9999, 9999));
	pointerGrid->SetLabelBackgroundColour(backgroundColor);
	pointerGrid->SetLabelTextColour(textColor);
	pointerGrid->SetDefaultCellBackgroundColour(foregroundColor);
	pointerGrid->SetDefaultCellTextColour(textColor);

	pointerGrid->CreateGrid(0, 1);
	pointerGrid->EnableGridLines(false);
	pointerGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	pointerGrid->SetScrollRate(0, 10);
	pointerGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	pointerGrid->DisableDragRowSize();
	pointerGrid->EnableEditing(false);
	pointerGrid->SetColLabelValue(0, "Base Address");
	pointerGrid->HideRowLabels();
	pointerGrid->SetColSize(0, 9999);
	pointerGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(levelsInputLabel, 0, wxRIGHT | wxLEFT, 10);
	row1Sizer->Add(levelsInput, 0, wxRIGHT, 15);
	row1Sizer->Add(maxOffsetInputLabel, 0, wxRIGHT | wxLEFT, 10);
	row1Sizer->Add(maxOffsetInput, 0, wxRIGHT, 15);

	row2Sizer->Add(resultsTxt, 0, wxALL, 10);
	row2Sizer->AddSpacer(175);
	row2Sizer->Add(scanButton, 0, wxALL, 10);

	vSizer->Add(addressInput, 0, wxCENTER | wxALL, 10);
	vSizer->Add(row1Sizer, 0, wxCENTER);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(pointerGrid, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);
}

void PointerScannerMenu::ScanButtonPress(wxCommandEvent& e)
{
	if (procHandle == 0 || procHandle == INVALID_HANDLE_VALUE)
	{
		wxMessageBox("Not attached to a process", "Can't scan for pointers");
		return;
	}
	
	uintptr_t address;
	if (!addressInput->GetValue().ToULongLong(&address, 16)) 
	{
		wxMessageBox("Invalid address", "Can't scan for pointers");
		return;
	}

	int levels;
	if (!levelsInput->GetValue().ToInt(&levels)) 
	{
		wxMessageBox("Invalid levels", "Can't scan for pointers");
		return;
	}
	if (levels < 1) { levels = 1; levelsInput->SetValue("1"); }

	int maxOffset;
	if (!maxOffsetInput->GetValue().ToInt(&maxOffset, 16)) 
	{
		wxMessageBox("Invalid max offset", "Can't scan for pointers");
		return;
	}
	
	BOOL isWow64Proc;
	IsWow64Process(procHandle, &isWow64Proc);
	bool is32Bit = isWow64Proc == TRUE;

	baseAddresses.clear();
	baseAddresses.shrink_to_fit();
	offsets.clear();
	offsets.shrink_to_fit();
	if (pointerGrid->GetNumberRows() > 0) { pointerGrid->DeleteRows(0, pointerGrid->GetNumberRows()); }
	if (pointerGrid->GetNumberCols() > 1) { pointerGrid->DeleteCols(1, pointerGrid->GetNumberCols() - 1); }
	
	std::vector<unsigned int> empty;
	unsigned int results = PointerScan(address, is32Bit, maxOffset, levels, empty);
	resultsTxt->SetLabelText("Results: " + CommaFormatNum(results));
	row2Sizer->RecalcSizes();

	UpdateModuleHandles();

	pointerGrid->AppendRows(results);

	pointerGrid->SetColSize(0, 200);
	pointerGrid->AppendCols(levels);
	for (int i = 0; i < levels; i++) 
	{ 
		pointerGrid->SetColLabelValue(i + 1, "Offset " + std::to_string(i+1)); 

		if (i == levels - 1) { pointerGrid->SetColSize(i + 1, 9999); }
		else { pointerGrid->SetColSize(i + 1, 75); }
	}

	for (int i = 0; i < results; i++) 
	{
		pointerGrid->SetCellValue(i, 0, FormatBaseAddress(baseAddresses[i]));
		pointerGrid->SetCellTextColour(i, 0, wxColour(30, 255, 30));

		wxString offsetsStr;
		for (int j = 0; j < levels; j++) 
		{
			std::stringstream offsetToHex;
			offsetToHex << std::hex << offsets[i][j];

			pointerGrid->SetCellValue(i, levels - j, offsetToHex.str());
		}
	}
}

unsigned int PointerScannerMenu::PointerScan(uintptr_t targetAddress, bool is32Bit, int maxOffset, int levels, std::vector<unsigned int> currentOffsets)
{
	unsigned int results = 0;
	
	int increment = is32Bit ? 4 : 8;
	DWORD targetType = levels == 1 ? MEM_IMAGE : MEM_PRIVATE;

	uintptr_t regionBaseAddress = 0;
	_MEMORY_BASIC_INFORMATION mbi;
	while (regionBaseAddress < 0x7fffffffffff && VirtualQueryEx(procHandle, (uintptr_t*)regionBaseAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT && mbi.Type == targetType)
		{
			unsigned char* buffer = new unsigned char[mbi.RegionSize];

			ReadProcessMemory(procHandle, (void*)regionBaseAddress, buffer, mbi.RegionSize, 0);

			for (int i = 0; i < mbi.RegionSize; i += increment)
			{
				uintptr_t value;
				
				if (is32Bit) { value = *(unsigned int*)(buffer + i); }
				else { value = *(uintptr_t*)(buffer + i); }

				if (value >= targetAddress - maxOffset && value <= targetAddress)
				{
					if (levels == 1) 
					{
						baseAddresses.push_back(regionBaseAddress + i);
						currentOffsets.push_back(targetAddress - value);
						offsets.push_back(currentOffsets);
						currentOffsets.pop_back();
						results++;
					}
					else 
					{
						currentOffsets.push_back(targetAddress - value);
						results += PointerScan(regionBaseAddress + i, is32Bit, maxOffset, levels - 1, currentOffsets);
						currentOffsets.pop_back();
					}
				}
			}

			delete[] buffer;
		}

		regionBaseAddress += mbi.RegionSize;
	}

	return results;
}

wxString PointerScannerMenu::FormatBaseAddress(uintptr_t baseAddress)
{
	for (int i = 0; i < moduleHandles.size(); i++)
	{
		if (baseAddress < moduleHandles[i]) { continue; }

		uintptr_t relativeAddress = baseAddress - moduleHandles[i];

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
				std::stringstream rvaToHex;
				rvaToHex << std::hex << relativeAddress;

				return "<" + moduleNames[i] + ">+" + rvaToHex.str();
			}
		}
	}

	return "";
}

void PointerScannerMenu::UpdateModuleHandles()
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

void PointerScannerMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right clicked on
	int col = e.GetCol();
	wxArrayInt selectedRows = pointerGrid->GetSelectedRows(); // all rows also selected

	wxMenuItem* cpyBaseAddr = new wxMenuItem(0, 202, "Copy Base Address");
	cpyBaseAddr->SetBackgroundColour(foregroundColor);
	cpyBaseAddr->SetTextColour(textColor);
	menu.Append(cpyBaseAddr);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(pointerGrid->GetCellValue(row, 0)); }, 202);

	wxMenuItem* cpyOffset = menu.Append(203, "Copy Offset");
	cpyOffset->SetBackgroundColour(foregroundColor);
	cpyOffset->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(pointerGrid->GetCellValue(row, col)); }, 203);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x + 10, pos.y + 110));
	e.Skip();
}

void PointerScannerMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void PointerScannerMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}