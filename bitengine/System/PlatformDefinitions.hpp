#ifndef BIT_PLATFORMDEFINITIONS_H
#define BIT_PLATFORMDEFINITIONS_H

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)

# define __OS_WIN__

#elif defined(linux) || defined(__linux)

# define __OS_UNIX__

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

# define __OS_MAC__

#else

# error Unknown Operating System

#endif

#endif