#include "cpd.h"

std::string cpd::Dir::AppDataDir(std::string const & appName)
{
#if defined(__CPD_MAC__) // MAC OS CODE HERE

    char* tmp = getenv("HOME");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "/Library/Application Support/" + appName + "/";

#elif defined(__CPD_UNIX__) // UNIX CODE HERE

    char* tmp = getenv("HOME");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "/." + appName + "/";

#elif defined(__CPD_WIN__) // WINDOWS CODE HERE

    char* tmp = getenv("APPDATA");

    if (tmp == 0) {
        return "";
    }

    return std::string(tmp) + "\\" + appName + "\\";

#endif
}


std::string cpd::Dir::ExecutablePath()
{
#if defined(__CPD_MAC__) // MAC OS CODE HERE

    return "";

#elif defined(__CPD_UNIX__) // UNIX CODE HERE

    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1); // "/proc/curproc/file" FreeBSD, "/proc/self/path/a.out" Solaris
    if (len != -1)
    {
        buff[len] = '\0';
        return std::string(buff);
    }
    else
    {
        return "";
    }

#elif defined(__CPD_WIN__) // WINDOWS CODE HERE

    return "";

#endif
}

std::string cpd::Dir::ExecutableDir()
{
#if defined(__CPD_MAC__) // MAC OS CODE HERE

    return "";

#elif defined(__CPD_UNIX__) // UNIX CODE HERE

    std::string full = cpd::Dir::ExecutablePath();
    size_t found = full.find_last_of("/\\");
    std::string dir = full.substr(0,found);
    //std::string file = str.substr(found+1);

    return dir;

#elif defined(__CPD_WIN__) // WINDOWS CODE HERE

    return "";

#endif
}
