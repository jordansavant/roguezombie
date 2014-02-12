#include "ResourcePath.h"
#include "cpd.h"

std::string resourcePath(void)
{
    return cpd::Dir::ExecutableDir() + "/content/";
}

