#include "Output.hpp"
#include <string>
#include <iostream>
#include <sstream>
#if defined(__OS_WIN__)
#include "windows.h"
#endif

void bit::Output::Debug(std::string message)
{
#if defined(__OS_WIN__)
    std::wstring stemp = std::wstring(message.begin(), message.end());
    const wchar_t* cst = stemp.c_str();
    OutputDebugStringW(cst);
    OutputDebugStringW(L"\n");
#else
    std::cout << message << std::endl;
#endif
}
