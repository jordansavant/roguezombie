#pragma once
#ifndef BIT_STRING_H
#define BIT_STRING_H

#include <string>
#include <vector>

namespace bit
{
    class String
    {
    public:

        static float parseFloat(const std::string& input);

        static std::vector<std::string> split(const std::string &s, char delim);
        
        static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    };
}

#endif