#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>

// contains functionality used by multible systems in the code

class Utils
{
public:
	enum ValueType 
	{
		Int64, Int32, Int16, Int8,
		UInt64, UInt32, UInt16, UInt8,
		Float, Double,
		Bytes, UTF8, UTF16
	};

	const int numberOfValueTypes = 13;
	const int numberOfNonArrayValueTypes = 10;
	
	const char* typeStrs[13] =
	{
		"Int64", "Int32", "Int16", "Int8",
		"UInt64", "UInt32", "UInt16", "UInt8",
		"Float", "Double",
		"Bytes", "UTF-8", "UTF-16"
	};

	const wxColour backgroundColor = wxColour(35, 35, 35);
	const wxColour foregroundColor = wxColour(60, 60, 60);
	const wxColour textColor = wxColour(220, 220, 220);

	const int updateRate = 150;

	void CopyToClipboard(const char* txt);

	wxString CommaFormatNum(unsigned int num);

	char* i64toa(unsigned long long val, char* sz, unsigned radix, int neg);
	char* lltoa(long long val, char* sz, unsigned radix);;
	char* ulltoa(unsigned long long val, char* sz, unsigned radix);
};