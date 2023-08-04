#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/clipbrd.h>
#include <sstream>

class Base
{
public:
	const char* typeStrs[6] =
	{
		"Int64", "Int32", "Int16", "Int8",
		"Float", "Double"
	};
	
	void WriteValue(HANDLE procHandle, unsigned long long* address, void* newValue, int size);

	void CopyToClipboard(const char* txt);
};