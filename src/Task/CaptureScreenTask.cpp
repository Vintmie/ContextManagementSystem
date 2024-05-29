#include "Task/CaptureScreenTask.h"
#include <Windows.h>
#include "FormatOutput.h"

ResultType CaptureScreenTask::getExecutionResult() const
{
    return currentExecutionResult;
}


ResultType CaptureScreenTask::execute(bool isLog)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return captureScreen(isLog);
}



ResultType CaptureScreenTask::captureScreen(bool isLog)
{
    auto res_logger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getThreadFileLogger(isLog);

    HDC hScreen = GetDC(NULL);
    HDC hDest = CreateCompatibleDC(hScreen);
    int width = GetDeviceCaps(hScreen, HORZRES);
    int height = GetDeviceCaps(hScreen, VERTRES);
    HBITMAP hbDesktop = CreateCompatibleBitmap(hScreen, width, height);
    HBITMAP hbOld = (HBITMAP)SelectObject(hDest, hbDesktop);
    BitBlt(hDest, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DeleteDC(hDest);
        ReleaseDC(NULL, hScreen);
        currentExecutionResult = ResultType::FAILURE;
        if (isLog != false)
        {
            res_logger->info("CaptureScreenTask returned: {}\n", currentExecutionResult);
        }
        file_logger->info("CaptureScreenTask returned: {}\n", currentExecutionResult, thread_id_str);
        return currentExecutionResult;
    }

    DWORD dwBytesWritten = 0;
    BITMAPFILEHEADER bmfHeader;
    bmfHeader.bfType = 0x4D42;  // BMP
    bmfHeader.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3);
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

    BYTE* lpBits = new BYTE[width * height * 3];
    GetDIBits(hDest, hbDesktop, 0, height, lpBits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    WriteFile(hFile, lpBits, width * height * 3, &dwBytesWritten, NULL);
    delete[] lpBits;

    CloseHandle(hFile);
    SelectObject(hDest, hbOld);
    DeleteObject(hbDesktop);
    DeleteDC(hDest);
    ReleaseDC(NULL, hScreen);

    currentExecutionResult = ResultType::SUCCESS;
    if (isLog != false)
    {
        res_logger->info("CaptureScreenTask returned: {}\n", currentExecutionResult);
    }
    file_logger->info("CaptureScreenTask returned: {}\n", currentExecutionResult, thread_id_str);

    return currentExecutionResult;
}

