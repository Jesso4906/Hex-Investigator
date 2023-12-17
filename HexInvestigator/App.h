#pragma once
#include <wx/wx.h>
#include "Main.h"

class App : public wxApp
{
public:
	virtual bool OnInit();

private:
	Main* frame1 = nullptr;
};