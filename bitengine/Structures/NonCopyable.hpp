#pragma once
#ifndef BIT_NONCOPYABLE_H
#define BIT_NONCOPYABLE_H

namespace bit
{
    class NonCopyable
    {

    protected:

        NonCopyable()
        {
        }
        
        ~NonCopyable()
        {
        }

    private:

        NonCopyable(const NonCopyable&);
        
        const NonCopyable& operator=(const NonCopyable&);

    };
}

#endif
