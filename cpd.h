/******************************************************************************
* CPD - Cross-Platform Directories
*
* File : cpd.hpp
*
* Copyright (C) 2010 Marco Antognini (antognini.marco@gmail.com)
*
* This software is provided 'as-is', without any express or implied warranty.
* In no event will the authors be held liable for any damages arising from
* the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
* you must not claim that you wrote the original software.
* If you use this software in a product, an acknowledgment
* in the product documentation would be appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such,
* and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*
*****************************************************************************/

/******************************************************************************
* Current Version : 1.0.3
* -----------------------
*
* Changelog :
* ===========
* 2010/09/17 : 1.0.0
* - Initial version
* - AppDataDir function added
*
* 2010/09/18 : 1.0.1
* - Fix includes missing
* - Fix return type of AppDataDir
* - Fix cygwin environment macro
*
* 2010/09/18 : 1.0.2
* - Fix OS X and Unix returned path
*
* 2010/09/20 : 1.0.3
* - Fix duplicate symbol problem
*
*****************************************************************************/

#ifndef __CPD_HPP__
#define __CPD_HPP__

/*
* Check current platform
*/
#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)

# define __CPD_WIN__

#elif defined(linux) || defined(__linux)

# define __CPD_UNIX__

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

# define __CPD_MAC__

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
namespace cpd
{
    class Dir
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

        static std::string ExecutablePath();

        static std::string ExecutableDir();
    };
}

#endif
