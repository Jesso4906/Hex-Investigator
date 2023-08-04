#include "App.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	frame1 = new Main();
	frame1->Show();

	return true;
}