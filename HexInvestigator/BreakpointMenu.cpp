#include "BreakpointMenu.h"

wxBEGIN_EVENT_TABLE(BreakpointMenu, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
EVT_BUTTON(AttachDebuggerID, AttachDebugger)
EVT_BUTTON(DetachDebuggerID, DetachDebugger)
EVT_BUTTON(RemoveHBPID, RemoveHBP)
EVT_BUTTON(SHBPWriteID, SetWriteBP)
EVT_BUTTON(SHBPReadWriteID, SetReadWriteBP)
EVT_BUTTON(SHBPExecuteID, SetExecuteBP)
EVT_TIMER(UpdateTimerID, UpdateList)
wxEND_EVENT_TABLE()

BreakpointMenu::BreakpointMenu(HANDLE procH) : wxFrame(nullptr, MainWindowID, "Breakpoint Menu", wxPoint(50, 50), wxSize(400, 400))
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	procHandle = procH;

	addressInput = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(310, 25));
	addressInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	addressInput->SetOwnForegroundColour(wxColour(220, 220, 220));
	addressInput->SetToolTip("Address to set a breakpoint on");
	addressInput->Disable();

	selectSize = new wxChoice(this, wxID_ANY, wxPoint(0, 0), wxSize(40, 25), wxArrayString(4, sizeStrs));
	selectSize->SetSelection(0);
	selectSize->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectSize->SetOwnForegroundColour(wxColour(220, 220, 220));
	selectSize->SetToolTip("Size of breakpoint");
	selectSize->Disable();

	attachDebugger = new wxButton(this, AttachDebuggerID, "Attach Debugger", wxPoint(0, 0), wxSize(100, 25));
	attachDebugger->SetOwnBackgroundColour(wxColour(60, 60, 60));
	attachDebugger->SetOwnForegroundColour(wxColour(220, 220, 220));

	detachDebugger = new wxButton(this, DetachDebuggerID, "Detach Debugger", wxPoint(0, 0), wxSize(100, 25));
	detachDebugger->SetOwnBackgroundColour(wxColour(60, 60, 60));
	detachDebugger->SetOwnForegroundColour(wxColour(220, 220, 220));
	detachDebugger->Disable();

	removeHBP = new wxButton(this, RemoveHBPID, "Remove HBP", wxPoint(0, 0), wxSize(100, 25));
	removeHBP->SetOwnBackgroundColour(wxColour(60, 60, 60));
	removeHBP->SetOwnForegroundColour(wxColour(220, 220, 220));
	removeHBP->SetToolTip("Remove hardware breakpoint");
	removeHBP->Disable();

	shbpWrite = new wxButton(this, SHBPWriteID, "Set HBP Write", wxPoint(0, 0), wxSize(100, 25));
	shbpWrite->SetOwnBackgroundColour(wxColour(60, 60, 60));
	shbpWrite->SetOwnForegroundColour(wxColour(220, 220, 220));
	shbpWrite->SetToolTip("Set hardware breakpoint write");
	shbpWrite->Disable();

	shbpReadWrite = new wxButton(this, SHBPReadWriteID, "Set HBP Read Write", wxPoint(0, 0), wxSize(120, 25));
	shbpReadWrite->SetOwnBackgroundColour(wxColour(60, 60, 60));
	shbpReadWrite->SetOwnForegroundColour(wxColour(220, 220, 220));
	shbpReadWrite->SetToolTip("Set hardware breakpoint read write");
	shbpReadWrite->Disable();

	shbpExecute = new wxButton(this, SHBPExecuteID, "Set HBP Execute", wxPoint(0, 0), wxSize(100, 25));
	shbpExecute->SetOwnBackgroundColour(wxColour(60, 60, 60));
	shbpExecute->SetOwnForegroundColour(wxColour(220, 220, 220));
	shbpExecute->SetToolTip("Set hardware breakpoint exectue");
	shbpExecute->Disable();

	addrList = new wxGrid(this, AddressListID, wxPoint(0, 0), wxSize(9999, 9999));
	addrList->SetLabelBackgroundColour(wxColour(40, 40, 40));
	addrList->SetLabelTextColour(wxColour(230, 230, 230));
	addrList->SetDefaultCellBackgroundColour(wxColour(60, 60, 60));
	addrList->SetDefaultCellTextColour(wxColour(220, 220, 220));

	addrList->CreateGrid(0, 3);
	addrList->EnableGridLines(false);
	addrList->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	addrList->SetScrollRate(0, 10);
	addrList->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	addrList->DisableDragRowSize();
	addrList->DisableDragColSize();
	addrList->EnableEditing(false);
	addrList->SetColLabelValue(0, "Address");
	addrList->SetColLabelValue(1, "Module");
	addrList->SetColLabelValue(2, "Hits");
	addrList->HideRowLabels();
	addrList->SetColSize(0, 100);
	addrList->SetColSize(1, 200);
	addrList->SetColSize(2, 9999);
	addrList->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	infoAboutAddresses = new wxStaticText(this, wxID_ANY, "*Addresses here correspond to the instruction after the hit instruction");
	infoAboutAddresses->SetOwnForegroundColour(wxColour(220, 220, 220));

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	updateListTimer = new wxTimer(this, UpdateTimerID);

	row1Sizer->Add(addressInput, 0, wxALL, 10);
	row1Sizer->Add(selectSize, 0, wxRight | wxTOP | wxBOTTOM, 10);

	row2Sizer->Add(attachDebugger, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);
	row2Sizer->Add(detachDebugger, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);
	row2Sizer->Add(removeHBP, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	row3Sizer->Add(shbpWrite, 0, wxCENTER | wxRIGHT | wxLEFT | wxBOTTOM, 10);
	row3Sizer->Add(shbpReadWrite, 0, wxCENTER | wxRIGHT | wxLEFT | wxBOTTOM, 10);
	row3Sizer->Add(shbpExecute, 0, wxCENTER | wxRIGHT | wxLEFT | wxBOTTOM, 10);

	vSizer->Add(infoAboutAddresses, 0, wxLEFT, 10);
	vSizer->Add(row1Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row2Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row3Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(addrList, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);

	SetSizer(vSizer);
}

//https://www.codeproject.com/Articles/28071/Toggle-hardware-data-read-execute-breakpoints-prog

bool BreakpointMenu::SetHardwareBreakpoint(uintptr_t address, BPSize size, BPType type)
{
	int procId = GetProcessId(procHandle);

	// set the debug registers in every thread that is a part of the process
	HANDLE threadSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));

	if (threadSnap != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(threadEntry);

		if (Thread32First(threadSnap, &threadEntry))
		{
			do
			{
				if (threadEntry.th32OwnerProcessID == procId)
				{
					HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

					CONTEXT ctx = { 0 };
					ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

					if (threadHandle == INVALID_HANDLE_VALUE || !GetThreadContext(threadHandle, &ctx))
					{
						return false;
					}

					ctx.Dr0 = address; // I only make use of the first debug register, becasue there will never be more than one HWBP set at a time
					ctx.Dr6 = 0; // reset status

					std::bitset<sizeof(ctx.Dr7) * 8> dr7; // array of bits
					std::memcpy(&dr7, &ctx.Dr7, sizeof(ctx.Dr7));

					dr7.set(0, true); // enable local debugging for the first register

					switch (type)
					{
					case BPType::ReadWrite:
						dr7.set(17, true);
						dr7.set(16, true);
						break;
					case BPType::Write:
						dr7.set(17, false);
						dr7.set(16, true);
						break;
					case BPType::Execute:
						dr7.set(17, false);
						dr7.set(16, false);
						break;
					}

					switch (size)
					{
					case BPSize::OneByte:
						dr7.set(19, false);
						dr7.set(18, false);
						break;
					case BPSize::TwoByte:
						dr7.set(19, false);
						dr7.set(18, true);
						break;
					case BPSize::FourByte:
						dr7.set(19, true);
						dr7.set(18, true);
						break;
					case BPSize::EightByte:
						dr7.set(19, true);
						dr7.set(18, false);
						break;
					}

					std::memcpy(&ctx.Dr7, &dr7, sizeof(ctx.Dr7));

					if (!SetThreadContext(threadHandle, &ctx))
					{
						return false;
					}

					CloseHandle(threadHandle);
				}
			} while (Thread32Next(threadSnap, &threadEntry));
		}
	}

	CloseHandle(threadSnap);

	currentAddress = address;
	currentSize = size;
	currentType = type;

	isBPSet = true;

	return true;
}

bool BreakpointMenu::DisableDebugRegisters()
{
	int procId = GetProcessId(procHandle);

	HANDLE threadSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));

	if (threadSnap != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(threadEntry);

		if (Thread32First(threadSnap, &threadEntry))
		{
			do
			{
				if (threadEntry.th32OwnerProcessID == procId)
				{
					HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

					CONTEXT ctx = { 0 };
					ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

					if (threadHandle == INVALID_HANDLE_VALUE || !GetThreadContext(threadHandle, &ctx))
					{
						return false;
					}

					ctx.Dr0 = 0;
					ctx.Dr6 = 0;
					ctx.Dr7 = 0;

					if (!SetThreadContext(threadHandle, &ctx))
					{
						return false;
					}

					CloseHandle(threadHandle);
				}
			} while (Thread32Next(threadSnap, &threadEntry));
		}
	}

	CloseHandle(threadSnap);

	return true;
}

void BreakpointMenu::AttachDebugger(wxCommandEvent& e)
{
	int procId = GetProcessId(procHandle);
	
	if (!DebugActiveProcess(procId)) // this is just a check to make sure it works. the debugger is attached in the thread.
	{
		wxMessageBox("Error attaching debugger", "Can't Attach Debugger");
		return;
	}
	DebugActiveProcessStop(procId);

	debugThread = new DebugThread(this, procHandle);

	if (debugThread->Create() != wxTHREAD_NO_ERROR)
	{
		wxMessageBox("Error creating thread", "Can't Attach Debugger");
		delete debugThread;
		return;
	}

	if (debugThread->Run() != wxTHREAD_NO_ERROR)
	{
		wxMessageBox("Error running thread", "Can't Attach Debugger");
		delete debugThread;
		return;
	}

	updateListTimer->Start(50);

	attachDebugger->Disable();
	detachDebugger->Enable();

	addressInput->Enable();
	selectSize->Enable();

	shbpWrite->Enable();
	shbpReadWrite->Enable();
	shbpExecute->Enable();

	isDebuggerAttached = true;
}

void BreakpointMenu::DetachDebugger(wxCommandEvent& e)
{
	if (!isDebuggerAttached) { return; }
	
	if (isBPSet) { RemoveHBP(e); }

	if (debugThread) 
	{ 
		DebugBreakProcess(procHandle); // this will break out of the WaitForDebugEvent infinite loop by triggering a debug event
		debugThread->Delete(); 
	}
	isDebuggerAttached = false;

	updateListTimer->Stop();

	shbpWrite->Disable();
	shbpReadWrite->Disable();
	shbpExecute->Disable();

	addressInput->SetValue("");
	addressInput->Disable();
	selectSize->Disable();

	attachDebugger->Enable();
	detachDebugger->Disable();
}

void BreakpointMenu::RemoveHBP(wxCommandEvent& e)
{
	if (!DisableDebugRegisters())
	{
		wxMessageBox("Error turning off Dr0", "Can't Remove Hardware Breakpoint");
		return;
	}
	isBPSet = false;

	removeHBP->Disable();

	addressInput->Enable();
	selectSize->Enable();

	shbpWrite->Enable();
	shbpReadWrite->Enable();
	shbpExecute->Enable();
}

void BreakpointMenu::SetWriteBP(wxCommandEvent& e)
{
	uintptr_t address;
	if (!addressInput->GetValue().ToULongLong(&address, 16)) 
	{
		wxMessageBox("Invalid Address", "Can't Set Breakpoint"); 
		return;
	}

	int size = selectSize->GetSelection();
	if (!SetHardwareBreakpoint(address, (BPSize)size, BPType::Write)) 
	{
		wxMessageBox("Error setting breakpoint", "Can't Set Breakpoint");
		return;
	}

	shbpWrite->Disable();
	shbpReadWrite->Disable();
	shbpExecute->Disable();

	attachDebugger->Disable();
	removeHBP->Enable();

	addressInput->Disable();
	selectSize->Disable();

	ClearList();
}

void BreakpointMenu::SetReadWriteBP(wxCommandEvent& e)
{
	uintptr_t address;
	if (!addressInput->GetValue().ToULongLong(&address, 16))
	{
		wxMessageBox("Invalid Address", "Can't Set Breakpoint");
		return;
	}

	int size = selectSize->GetSelection();
	if (!SetHardwareBreakpoint(address, (BPSize)size, BPType::ReadWrite))
	{
		wxMessageBox("Error setting breakpoint", "Can't Set Breakpoint");
		return;
	}

	shbpWrite->Disable();
	shbpReadWrite->Disable();
	shbpExecute->Disable();

	attachDebugger->Disable();
	removeHBP->Enable();

	addressInput->Disable();
	selectSize->Disable();

	ClearList();
}

void BreakpointMenu::SetExecuteBP(wxCommandEvent& e)
{
	uintptr_t address;
	if (!addressInput->GetValue().ToULongLong(&address, 16))
	{
		wxMessageBox("Invalid Address", "Can't Set Breakpoint");
		return;
	}

	int size = selectSize->GetSelection();
	if (!SetHardwareBreakpoint(address, (BPSize)size, BPType::Execute))
	{
		wxMessageBox("Error setting breakpoint", "Can't Set Breakpoint");
		return;
	}

	shbpWrite->Disable();
	shbpReadWrite->Disable();
	shbpExecute->Disable();

	attachDebugger->Disable();
	removeHBP->Enable();

	addressInput->Disable();
	selectSize->Disable();

	ClearList();
}

void BreakpointMenu::AddAddressToList(uintptr_t address)
{
	// if it's already in the list then increment the hits
	for (int i = 0; i < entries.size(); i++) 
	{
		if (entries[i].address == address) 
		{
			entries[i].hits++;
			return;
		}
	}

	ListEntry entry;
	entry.address = address;

	entry.moduleName = "unknown";
	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(procHandle));
	if (modSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(modSnap, &modEntry))
		{
			do
			{
				if (address >= (uintptr_t)modEntry.modBaseAddr && address <= (((uintptr_t)modEntry.modBaseAddr) + modEntry.modBaseSize)) 
				{
					entry.moduleName = wxString(modEntry.szModule);
					break;
				}

			} while (Module32Next(modSnap, &modEntry));
		}
	}
	CloseHandle(modSnap);

	entry.hits = 1;

	entries.push_back(entry);

	addrList->AppendRows(1);
}

void BreakpointMenu::UpdateList(wxTimerEvent& e) // if AddAddressToList called SetCellValue instead it will crash the program if it is called to often
{
	for (int i = 0; i < entries.size(); i++)
	{
		std::stringstream addressToHex;
		addressToHex << std::hex << entries[i].address;
		addrList->SetCellValue(i, 0, addressToHex.str());

		addrList->SetCellValue(i, 1, entries[i].moduleName);

		addrList->SetCellValue(i, 2, std::to_string(entries[i].hits));
	}
}

void BreakpointMenu::ClearList() 
{
	int rows = addrList->GetNumberRows();
	if (rows > 0) { addrList->DeleteRows(0, rows); }
	entries.clear();
	entries.shrink_to_fit();
}

void BreakpointMenu::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow();

	wxMenuItem* cpyAddr = menu.Append(202, "Copy Address");
	cpyAddr->SetBackgroundColour(wxColour(60, 60, 60));
	cpyAddr->SetTextColour(wxColour(220, 220, 220));
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(addrList->GetCellValue(row, 0)); }, 202);

	wxPoint pos = e.GetPosition();
	PopupMenu(&menu, wxPoint(pos.x+10, pos.y+112));
	e.Skip();
}

void BreakpointMenu::OpenMenu(wxPoint position) 
{
	SetPosition(position);
	Show();
}

void BreakpointMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	wxCommandEvent e2; DetachDebugger(e2);
	Hide();
}

// Debug thread

DebugThread::DebugThread(BreakpointMenu* bpMenu, HANDLE proc) : wxThread()
{
	breakpointMenu = bpMenu;
	procHandle = proc;
}

wxThread::ExitCode DebugThread::Entry()
{
	SetName("Debug Thread");

	DebugActiveProcess(GetProcessId(procHandle));

	DebugSetProcessKillOnExit(false); // dont kill the process when debugger is detached

	while (!TestDestroy())
	{
		DEBUG_EVENT debugEvent;

		WaitForDebugEvent(&debugEvent, INFINITE);

		DWORD exceptionCode = debugEvent.u.Exception.ExceptionRecord.ExceptionCode;
		if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT && (exceptionCode == STATUS_WX86_SINGLE_STEP || exceptionCode == EXCEPTION_SINGLE_STEP))
		{
			breakpointMenu->AddAddressToList((uintptr_t)debugEvent.u.Exception.ExceptionRecord.ExceptionAddress);
		}

		ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
	}

	DebugActiveProcessStop(GetProcessId(procHandle));

	return (wxThread::ExitCode)0;
}