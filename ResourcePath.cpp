#include "ResourcePath.h"
#include "bitengine/System/FileSystem.hpp"

std::string resourcePath(void)
{
    return bit::FileSystem::ExecutableDirectory() + "/content/";
}