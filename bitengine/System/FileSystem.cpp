#include "FileSystem.hpp"

#if defined(__OS_WIN__)
#include "windows.h"
#endif

std::string bit::FileSystem::AppDataDir(std::string const & appName)
{
#if defined(__OS_MAC__) // MAC OS CODE

    char* tmp = getenv("HOME");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "/Library/Application Support/" + appName + "/";

#elif defined(__OS_UNIX__) // UNIX CODE

    char* tmp = getenv("HOME");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "/." + appName + "/";

#elif defined(__OS_WIN__) // WINDOWS CODE

    char* tmp = getenv("APPDATA");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "\\" + appName + "\\";

#endif
}

std::string bit::FileSystem::ExecutablePath()
{
#if defined(__OS_MAC__) // MAC OS CODE

    return "";

#elif defined(__OS_UNIX__) // UNIX CODE

    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    // "/proc/curproc/file" FreeBSD, "/proc/self/path/a.out" Solaris
    if (len != -1)
    {
        buff[len] = '\0';
        return std::string(buff);
    }
    else
    {
        return "";
    }

#elif defined(__OS_WIN__) // WINDOWS CODE

    TCHAR szPath[MAX_PATH];
    int charcount = GetModuleFileName(NULL, szPath, MAX_PATH);
    if(charcount > 0)
    {
        std::wstring arr_w(szPath);
        std::string arr_s(arr_w.begin(), arr_w.end());
        return arr_s;
    }

    return "";

#endif
}

std::string bit::FileSystem::ExecutableDirectory()
{
#if defined(__OS_MAC__) // MAC OS CODE

    return "";

#elif defined(__OS_UNIX__) // UNIX CODE

    std::string full = bit::FileSystem::ExecutablePath();
    size_t found = full.find_last_of("/\\");
    std::string dir = full.substr(0,found);

    return dir;

#elif defined(__OS_WIN__) // WINDOWS CODE

    std::string full = bit::FileSystem::ExecutablePath();
    size_t found = full.find_last_of("/\\");
    std::string dir = full.substr(0,found);

    return dir;

#endif
}

std::string bit::FileSystem::ExecutableFilename()
{
#if defined(__OS_MAC__) // MAC OS CODE

    return "";

#elif defined(__OS_UNIX__) // UNIX CODE

    std::string full = bit::FileSystem::ExecutablePath();
    size_t found = full.find_last_of("/\\");
    std::string file = full.substr(found+1);

    return file;

#elif defined(__OS_WIN__) // WINDOWS CODE

    std::string full = bit::FileSystem::ExecutablePath();
    size_t found = full.find_last_of("/\\");
    std::string file = full.substr(found+1);

    return file;

#endif
}
