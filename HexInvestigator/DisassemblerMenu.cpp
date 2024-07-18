#include "DisassemblerMenu.h"

wxBEGIN_EVENT_TABLE(DisassemblerMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(DisassembleButtonID, DisassembleButtonPress)
wxEND_EVENT_TABLE()

DisassemblerMenu::DisassemblerMenu(HANDLE procH) : wxFrame(nullptr, MainWindowID, "Disassembler", wxPoint(50, 50), wxSize(600, 600))
{
	this->SetOwnBackgroundColour(backgroundColor);

	procHandle = procH;

	infoTxt = new wxStaticText(this, wxID_ANY, "Input an address to disassemble. The instructions surrounding the address will also be disassembled.");
	infoTxt->SetOwnForegroundColour(textColor);

	addressInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(300, 25));
	addressInput->SetOwnBackgroundColour(foregroundColor);
	addressInput->SetOwnForegroundColour(textColor);

	disassembleButton = new wxButton(this, DisassembleButtonID, "Disassemble", wxPoint(0, 0), wxSize(100, 25));
	disassembleButton->SetOwnBackgroundColour(foregroundColor);
	disassembleButton->SetOwnForegroundColour(textColor);

	disassembly = new wxGrid(this, DisassemblyID, wxPoint(0, 0), wxSize(9999, 9999));
	disassembly->SetLabelBackgroundColour(backgroundColor);
	disassembly->SetLabelTextColour(textColor);
	disassembly->SetDefaultCellBackgroundColour(foregroundColor);
	disassembly->SetDefaultCellTextColour(textColor);

	disassembly->CreateGrid(0, 3);
	disassembly->EnableGridLines(false);
	disassembly->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	disassembly->SetScrollRate(0, 10);
	disassembly->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	disassembly->DisableDragRowSize();
	disassembly->EnableEditing(false);
	disassembly->SetColLabelValue(0, "Address");
	disassembly->SetColLabelValue(1, "RVA");
	disassembly->SetColLabelValue(2, "Disassembly");
	disassembly->HideRowLabels();
	disassembly->SetColSize(0, 100);
	disassembly->SetColSize(1, 200);
	disassembly->SetColSize(2, 9999);
	disassembly->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(infoTxt, 0, wxCENTER | wxTOP, 10);
	vSizer->Add(addressInput, 0, wxCENTER | wxALL, 10);
	vSizer->Add(disassembleButton, 0, wxCENTER | wxBOTTOM, 10);
	vSizer->Add(disassembly, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);
}

void DisassemblerMenu::DisassembleButtonPress(wxCommandEvent& e)
{
	int selection = e.GetSelection();

	if (disassembly->GetNumberRows() > 0) { disassembly->DeleteRows(0, disassembly->GetNumberRows()); }

	uintptr_t address = 0;
	if (!addressInput->GetValue().ToULongLong(&address, 16)) { wxMessageBox("Invalid address", "Can't disassemble"); return; }

	Disassemble(address);
}

void DisassemblerMenu::Disassemble(uintptr_t address)
{
	if (procHandle == 0 || procHandle == INVALID_HANDLE_VALUE)
	{
		wxMessageBox("Not attached to a process", "Can't disassemble");
		return;
	}
	
	BYTE* data = nullptr;
	
	ModuleCodeSection codeSection = {};
	for (int i = 0; i < moduleCodeSections.size(); i++)
	{
		if (moduleCodeSections[i].start <= address && moduleCodeSections[i].end >= address)
		{
			codeSection = moduleCodeSections[i];

			data = new BYTE[codeSection.size];

			break;
		}
	}

	if (data == nullptr) 
	{ 
		wxMessageBox("Failed to find code section for the input address", "Couldn't find code section for address");
		return; 
	}

	uintptr_t currentAddress = codeSection.start;

	if (currentAddress == 0 || ReadProcessMemory(procHandle, (void*)currentAddress, data, codeSection.size, nullptr) == FALSE)
	{ 
		wxMessageBox("Failed to read process memory", "Can't disassemble");
		return;
	}

	int rowOfAddress = 0;
	int row = 0;

	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32);

	ZydisFormatter formatter;
	ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	while (currentAddress <= codeSection.end)
	{
		ZydisDecoderContext ctx;
		ZydisDecoderDecodeInstruction(&decoder, &ctx, data + offset, codeSection.size - offset, &instruction.info);
		
		if (currentAddress < address - 0x100 || currentAddress > address + 0x100) 
		{ 
			offset += instruction.info.length;
			currentAddress += instruction.info.length;

			continue; 
		}

		ZydisDecoderDecodeOperands(&decoder, &ctx, &instruction.info, instruction.operands, instruction.info.operand_count);
		ZydisFormatterFormatInstruction(&formatter, &instruction.info, instruction.operands, instruction.info.operand_count_visible, instruction.text, sizeof(instruction.text), currentAddress, ZYAN_NULL);

		disassembly->AppendRows(1);

		std::stringstream addrToHex;
		addrToHex << std::hex << currentAddress;
		disassembly->SetCellValue(row, 0, addrToHex.str());

		std::stringstream rvaToHex;
		rvaToHex << std::hex << (currentAddress - codeSection.moduleBaseAddress);
		disassembly->SetCellValue(row, 1, codeSection.moduleName + rvaToHex.str());

		disassembly->SetCellValue(row, 2, instruction.text);

		if (currentAddress == address) 
		{
			rowOfAddress = row;

			disassembly->SetCellTextColour(row, 0, wxColour(255, 30, 30));
			disassembly->SetCellTextColour(row, 1, wxColour(255, 30, 30));
			disassembly->SetCellTextColour(row, 2, wxColour(255, 30, 30));
		}

		offset += instruction.info.length;
		currentAddress += instruction.info.length;

		row++;
	}

	disassembly->GoToCell(rowOfAddress, 0);

	delete[] data;
}

void DisassemblerMenu::UpdateModuleAddresses()
{
	moduleCodeSections.clear();
	moduleCodeSections.shrink_to_fit();

	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(procHandle));

	if (modSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(modSnap, &modEntry))
		{
			do
			{
				IMAGE_DOS_HEADER dosHeader = {};
				ReadProcessMemory(procHandle, modEntry.hModule, &dosHeader, sizeof(dosHeader), nullptr);

				uintptr_t imageNtHeadersAddress = ((uintptr_t)modEntry.hModule + (uintptr_t)dosHeader.e_lfanew);

				IMAGE_NT_HEADERS imageNtHeader = {};
				ReadProcessMemory(procHandle, (uintptr_t*)imageNtHeadersAddress, &imageNtHeader, sizeof(imageNtHeader), nullptr);

				ModuleCodeSection moduleCodeSection = {};

				for (int j = 0; j < imageNtHeader.FileHeader.NumberOfSections; j++)
				{
					IMAGE_SECTION_HEADER section = {};

					uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * j) + imageNtHeadersAddress + sizeof(imageNtHeader.Signature) + sizeof(imageNtHeader.FileHeader) + imageNtHeader.FileHeader.SizeOfOptionalHeader;

					ReadProcessMemory(procHandle, (uintptr_t*)sectionAddress, &section, sizeof(section), nullptr);

					if (section.Characteristics & IMAGE_SCN_CNT_CODE) 
					{
						uintptr_t sectionBase = (uintptr_t)section.VirtualAddress + (uintptr_t)modEntry.modBaseAddr;
						uintptr_t sectionEnd = sectionBase + (uintptr_t)section.Misc.VirtualSize;
						
						moduleCodeSection.start = sectionBase;
						moduleCodeSection.end = sectionEnd;
						moduleCodeSection.size = (uintptr_t)section.Misc.VirtualSize;
						moduleCodeSection.moduleBaseAddress = (uintptr_t)modEntry.modBaseAddr;
						moduleCodeSection.moduleName = "<" + wxString(modEntry.szModule) + ">+";
						moduleCodeSections.push_back(moduleCodeSection);
						break;
					}
				}

			} while (Module32Next(modSnap, &modEntry));
		}
	}
	CloseHandle(modSnap);
}

void DisassemblerMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right clicked on
	wxArrayInt selectedRows = disassembly->GetSelectedRows(); // all rows also selected

	wxMenuItem* cpyAddr = new wxMenuItem(0, 202, "Copy Address");
	cpyAddr->SetBackgroundColour(foregroundColor);
	cpyAddr->SetTextColour(textColor);
	menu.Append(cpyAddr);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassembly->GetCellValue(row, 0)); }, 202);

	wxMenuItem* cpyRVA = menu.Append(203, "Copy RVA");
	cpyRVA->SetBackgroundColour(foregroundColor);
	cpyRVA->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassembly->GetCellValue(row, 1)); }, 203);

	wxMenuItem* cpyVal = menu.Append(204, "Copy Disassembly");
	cpyVal->SetBackgroundColour(foregroundColor);
	cpyVal->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassembly->GetCellValue(row, 2)); }, 204);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x + 10, pos.y + 90));
	e.Skip();
}

void DisassemblerMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
	UpdateModuleAddresses();
}

void DisassemblerMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}