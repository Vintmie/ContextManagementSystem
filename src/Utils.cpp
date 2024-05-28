#include "Utils.h"
#include <commdlg.h>
#include <iostream>
#include <comutil.h>

std::wstring Utils::OpenFileSelectionDialog(DWORD flags, LPCWSTR title, LPCWSTR filter)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = flags;

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    else
    {
        DWORD error = CommDlgExtendedError();
        if (error != 0)
        {
            std::cerr << "Error occurred: " << error << std::endl;
        }
        return L"";
    }
}

std::wstring Utils::SaveFileSelectionDialog(DWORD flags, LPCWSTR title, LPCWSTR filter)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = flags;

    if (GetSaveFileNameW(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    else
    {
        DWORD error = CommDlgExtendedError();
        if (error != 0)
        {
            std::cerr << "Error occurred: " << error << std::endl;
        }
        return L"";
    }
}

std::string Utils::wstring_to_utf8(const std::wstring& str)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &utf8_str[0], size_needed, NULL, NULL);
    return utf8_str;
}

std::wstring Utils::utf8_to_wstring(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring w_str(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &w_str[0], size_needed);
    return w_str;
}

BSTR Utils::stdWstringToBstr(const std::wstring& str)
{
    return _bstr_t(str.c_str()).GetBSTR();
}