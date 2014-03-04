#pragma once
#ifndef BIT_EXCEPTION_H
#define BIT_EXCEPTION_H

#include <exception>

namespace bit
{
    class Exception : public std::exception
    {
    public:

        Exception(std::string _message)
        {
            message = _message;
        }

        virtual ~Exception() throw ()
        {
        }

        std::string message;

        virtual const char* what() const throw()
        {
            return message.c_str();
        }

        std::string getMessage()
        {
            return message;
        }
    };
}

#endif