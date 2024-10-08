#include "Utils.h"

void Utils::CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}

wxString Utils::CommaFormatNum(unsigned int num)
{
	std::string numStr = std::to_string(num);
	int n = numStr.length() - 3;
	while (n > 0)
	{
		numStr.insert(n, ",");
		n -= 3;
	}

	return numStr;
}

//https://www.prowaretech.com/articles/current/c-plus-plus/procedures/itoa
// 
// 64-BIT NUMBER TO ASCII (USED BY LLTOA AND ULLTOA)
char* Utils::i64toa(unsigned long long val, char* sz, unsigned radix, int neg)
{
	char* p = sz;
	char* first;
	unsigned long long dig;

	if (neg)
	{
		val = (unsigned long long)(-(long long)val);
		*p++ = '-';
	}

	first = p;

	do {
		dig = (unsigned)(val % radix);
		val /= radix;

		if (dig > 9)
			*p++ = (char)(dig - 10 + 'a');  // a letter
		else
			*p++ = (char)(dig + '0');       // a digit
	} while (val > 0);

	*p-- = '\0';

	char tmp;

	do {
		tmp = *p;
		*p = *first;
		*first = tmp;
		--p;
		++first;
	} while (first < p);
	return sz;
}

// CONVERT SIGNED LONG LONG TO ASCII
char* Utils::lltoa(long long val, char* sz, unsigned radix)
{
	return i64toa((unsigned long long)val, sz, radix, radix == 10 && val < 0);
}

// CONVERT UNSIGNED LONG LONG TO ASCII
char* Utils::ulltoa(unsigned long long val, char* sz, unsigned radix) // specify the radix of 36 to make alpha-numeric c-string number
{
	return i64toa(val, sz, radix, 0);
}
