#pragma once
#include "Utils.h"
#include <sstream>

class HexCalculator : public wxFrame, public Utils
{
public:
	HexCalculator();

	wxTextCtrl* firstValueInput = nullptr;
	wxChoice* selectOperation = nullptr;
	wxTextCtrl* secondValueInput = nullptr;

	wxButton* copyHexResult = nullptr;
	wxStaticText* hexResultTxt = nullptr;

	wxButton* copyDecResult = nullptr;
	wxStaticText* decResultTxt = nullptr;

	// these are for copying to clipboard
	wxString hexResultStr;
	wxString decResultStr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID,
		FirstValueInputID,
		SelectOperationID,
		SecondValueInputID,
		CopyHexResultID,
		CopyDecResultID
	};

	const char* operationChars[4]
	{
		"+",
		"-",
		"*",
		"/"
	};

	void CalculateResult(wxCommandEvent& e);

	void CopyHexResult(wxCommandEvent& e);

	void CopyDecResult(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};