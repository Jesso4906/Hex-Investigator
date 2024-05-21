#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>

// This just contains functionality used by multible systems in the code

class Utils
{
public:
	const char* typeStrs[11] =
	{
		"Int64", "Int32", "Int16", "Int8",
		"UInt64", "UInt32", "UInt16", "UInt8",
		"Float", "Double",
		"Bytes"
	};

	const int updateRate = 150;

	void WriteValue(HANDLE procHandle, uintptr_t* address, void* newValue, int size);

	void CopyToClipboard(const char* txt);


	char* i64toa(unsigned long long val, char* sz, unsigned radix, int neg);
	char* lltoa(long long val, char* sz, unsigned radix);;
	char* ulltoa(unsigned long long val, char* sz, unsigned radix);
};

