#pragma once
#include "Utils.h"
#include <sstream>
#include <wx/numdlg.h> 
#include <wx/grid.h>
#include "TLHelp32.h"
#include "Zydis.h"

class DisassemblerMenu : public wxFrame, public Utils
{
public:
	DisassemblerMenu(HANDLE);

	wxTextCtrl* addressInput = nullptr;
	wxButton* disassembleButton = nullptr;
	wxGrid* disassembly = nullptr;

	wxStaticText* infoTxt = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID,
		DisassembleButtonID,
		DisassemblyID
	};

	HANDLE procHandle;

	struct ModuleCodeSection
	{
		uintptr_t start, end, size, moduleBaseAddress;
		wxString moduleName;
	};
	std::vector<ModuleCodeSection> moduleCodeSections;

	void DisassembleButtonPress(wxCommandEvent& e);

	void Disassemble(uintptr_t address);

	void UpdateModuleAddresses();

	void RightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};