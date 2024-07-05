#include "HexCalculator.h"

wxBEGIN_EVENT_TABLE(HexCalculator, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_TEXT(FirstValueInputID, CalculateResult)
EVT_TEXT(SecondValueInputID, CalculateResult)
EVT_CHOICE(SelectOperationID, CalculateResult)
EVT_BUTTON(CopyHexResultID, CopyHexResult)
EVT_BUTTON(CopyDecResultID, CopyDecResult)
wxEND_EVENT_TABLE()

HexCalculator::HexCalculator() : wxFrame(nullptr, MainWindowID, "Hex Calculator", wxPoint(50, 50), wxSize(380, 150))
{
	this->SetOwnBackgroundColour(wxColour(35, 35, 35));

	firstValueInput = new wxTextCtrl(this, FirstValueInputID, "C", wxPoint(0, 0), wxSize(150, 25));
	firstValueInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	firstValueInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	selectOperation = new wxChoice(this, SelectOperationID, wxPoint(0, 0), wxSize(30, 25), wxArrayString(4, operationChars));
	selectOperation->SetSelection(0);
	selectOperation->SetOwnBackgroundColour(wxColour(60, 60, 60));
	selectOperation->SetOwnForegroundColour(wxColour(220, 220, 220));

	secondValueInput = new wxTextCtrl(this, SecondValueInputID, "4", wxPoint(0, 0), wxSize(150, 25));
	secondValueInput->SetOwnBackgroundColour(wxColour(60, 60, 60));
	secondValueInput->SetOwnForegroundColour(wxColour(220, 220, 220));

	copyHexResult = new wxButton(this, CopyHexResultID, "Copy", wxPoint(0, 0), wxSize(50, 25));
	copyHexResult->SetOwnBackgroundColour(wxColour(60, 60, 60));
	copyHexResult->SetOwnForegroundColour(wxColour(220, 220, 220));

	hexResultTxt = new wxStaticText(this, wxID_ANY, "Hex Result: 10");
	hexResultTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	copyDecResult = new wxButton(this, CopyDecResultID, "Copy", wxPoint(0, 0), wxSize(50, 25));
	copyDecResult->SetOwnBackgroundColour(wxColour(60, 60, 60));
	copyDecResult->SetOwnForegroundColour(wxColour(220, 220, 220));

	decResultTxt = new wxStaticText(this, wxID_ANY, "Dec Result: 16");
	decResultTxt->SetOwnForegroundColour(wxColour(220, 220, 220));

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(firstValueInput, 0, wxALL, 10);
	row1Sizer->Add(selectOperation, 0, wxRight | wxTOP | wxBOTTOM, 10);
	row1Sizer->Add(secondValueInput, 0, wxALL, 10);

	row2Sizer->Add(copyHexResult, 0, wxLEFT | wxBOTTOM, 10);
	row2Sizer->Add(hexResultTxt, 0, wxALL, 5);

	row3Sizer->Add(copyDecResult, 0, wxLEFT | wxBOTTOM, 10);
	row3Sizer->Add(decResultTxt, 0, wxALL, 5);

	vSizer->Add(row1Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row2Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row3Sizer, 0, wxEXPAND | wxCENTER);

	SetSizer(vSizer);
}

void HexCalculator::CalculateResult(wxCommandEvent& e)
{
	unsigned long long firstValue = 0;
	unsigned long long secondValue = 0;

	if (!firstValueInput->GetValue().ToULongLong(&firstValue, 16) || !secondValueInput->GetValue().ToULongLong(&secondValue, 16))
	{
		hexResultTxt->SetLabelText("Hex Result: ");
		decResultTxt->SetLabelText("Dec Result: ");
		return;
	}

	unsigned long long result;
	bool validValues = true;

	switch (selectOperation->GetSelection()) 
	{
	case 0: // +
		result = firstValue + secondValue;
		break;
	case 1: // -
		if (secondValue > firstValue) { validValues = false; break; }
		result = firstValue - secondValue;
		break;
	case 2: // *
		result = firstValue * secondValue;
		break;
	case 3: // /
		if (secondValue > firstValue || secondValue == 0) { validValues = false; break; }
		result = firstValue / secondValue;
		break;
	}

	if (!validValues) 
	{
		hexResultTxt->SetLabelText("Hex Result: ");
		decResultTxt->SetLabelText("Dec Result: ");
		return;
	}

	std::stringstream resultToHex;
	resultToHex << std::hex << result;
	hexResultStr = resultToHex.str();
	hexResultTxt->SetLabelText("Hex Result: " + hexResultStr);

	decResultStr = std::to_string(result);
	decResultTxt->SetLabelText("Dec Result: " + decResultStr);
}

void HexCalculator::CopyHexResult(wxCommandEvent& e)
{
	CopyToClipboard(hexResultStr.c_str());
}

void HexCalculator::CopyDecResult(wxCommandEvent& e)
{
	CopyToClipboard(decResultStr.c_str());
}

void HexCalculator::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void HexCalculator::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}