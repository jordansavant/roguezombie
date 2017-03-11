#include "Output.hpp"
#include <string>
#include <iostream>
#include <sstream>
#if defined(__OS_WIN__)
#include "windows.h"
#endif

std::stringstream bit::Output::stream;

void bit::Output::Debug(unsigned int integer)
{
    std::stringstream ss;
    ss << integer;
    Debug(ss.str());
}

void bit::Output::Debug(int integer)
{
    std::stringstream ss;
    ss << integer;
    Debug(ss.str());
}

void bit::Output::Debug(float floatVal)
{
    std::stringstream ss;
    ss << floatVal;
    Debug(ss.str());
}

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

void bit::Output::Flush()
{
    Debug(stream.str());
    stream.str("");
}