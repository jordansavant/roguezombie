#include "String.hpp"
#include "../Math/Math.hpp"
#include <vector>
#include <sstream>
#include <string>

float bit::String::parseFloat(const std::string& input)
{
    const char *p = input.c_str();
    if (!*p || *p == '?')
        return 0;
    int s = 1;
    while (*p == ' ') p++;

    if (*p == '-') {
        s = -1; p++;
    }

    double acc = 0;
    while (*p >= '0' && *p <= '9')
        acc = acc * 10 + *p++ - '0';

    if (*p == '.') {
        double k = 0.1;
        p++;
        while (*p >= '0' && *p <= '9') {
            acc += (*p++ - '0') * k;
            k *= 0.1;
        }
    }
    if (*p) return 0; //die("Invalid numeric format");
    return s * acc;
}

std::vector<std::string>& bit::String::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> bit::String::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

unsigned int bit::String::toHash(std::string &st)
{
    return bit::Math::toHash(st);
}