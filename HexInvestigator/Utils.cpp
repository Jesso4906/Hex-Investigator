#include "Utils.h"

void Utils::WriteValue(HANDLE procHandle, unsigned long long* address, void* newValue, int size)
{
	DWORD oldProtect;
	VirtualProtectEx(procHandle, address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(procHandle, address, newValue, size, nullptr);
	VirtualProtectEx(procHandle, address, size, oldProtect, &oldProtect);
}

void Utils::CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}