#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <codecvt>

class Utils
{
public:
    static std::wstring OpenFileSelectionDialog(DWORD flags, LPCWSTR title, LPCWSTR filter = L"All Files\0*.*\0");
    static std::wstring SaveFileSelectionDialog(DWORD flags, LPCWSTR title, LPCWSTR filter = L"All Files\0*.*\0");
    static std::string wstring_to_utf8(const std::wstring& str);
    static std::wstring utf8_to_wstring(const std::string& str);
    static BSTR stdWstringToBstr(const std::wstring& str);
};
