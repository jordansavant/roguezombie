#pragma once
#ifndef BIT_FILESYSTEM_H
#define BIT_FILESYSTEM_H

/*
* Check current platform
*/
#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)

# define __OS_WIN__

#elif defined(linux) || defined(__linux)

# define __OS_UNIX__

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

# define __OS_MAC__

#else

# error Unknown Operating System

#endif

/*
* Includes
*/
#include <string>
#include <cstdlib>

/**
* @brief CROSS-PLATFORM DIRECTORIES
*
* Some functions to get platform specific directories.
**/
namespace bit
{
    class FileSystem
    {
    public:
        /**
         * Returns the directory for appName configuration/data storage.
         *
         * <pre>
         * Unix-like "/home/username/.appName/"
         * Mac OS X "/Users/username/Library/Application Support/appName/"
         * Windows XP "C:\Documents and Settings\username\Application Data\appinfo\"
         * Windows Vista & 7 "C:\Users\username\AppData\Roaming\appinfo\"
         * </pre>
         *
         * @note Windows version uses the environment variable %%APPDATA%.
         *
         * @note Unix and Mac versions use $HOME variable.
         *
         * @param appName
         * The name of the application.
         *
         * @return Directory for your application configuration and data storage
         * or "" (an empty string) if something went wrong.
         *
         * @warning You may have to create the directory.
         **/
        static std::string AppDataDir(std::string const & appName);

        /**
         * Returns full path and filename of the current running executable.
         **/
        static std::string ExecutablePath();

        /**
         * Returns full directory path of current running executable.
         **/
        static std::string ExecutableDirectory();

        /**
         * Returns only the filename of the executable
         **/
        static std::string ExecutableFilename();
    };
}

#endif
