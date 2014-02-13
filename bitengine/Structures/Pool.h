#pragma once
#ifndef BIT_POOL_H
#define BIT_POOL_H

#include <vector>
#include <functional>

namespace bit
{
    template<class T>
    class Pool
    {
    public:
        Pool()
            : maximumCreated(0), factoryMethod(NULL)
        {
        }

	    Pool(int count)
            : maximumCreated(0), factoryMethod(NULL)
	    {
		    add(count);
	    }

        Pool(int count, std::function<T*(void)> factoryMethod)
            : maximumCreated(0), factoryMethod(factoryMethod)
	    {
		    add(count);
	    }


	    ~Pool()
	    {
            for(int i = 0; i < pool.size(); i++)
		    {
			    delete pool[i];
		    }
	    }

	    std::vector<T*> pool;
        unsigned int maximumCreated;
        std::function<T*(void)> factoryMethod;

	    void add(int count)
	    {
            for(int i = 0; i < count; i++)
		    {
			    T* ob = create();
			    pool.push_back(ob);
                maximumCreated = std::max(maximumCreated, pool.size());
		    }
	    }

	    T* fetch()
	    {
            T* ob = NULL;

            if(pool.size() > 0)
            {
                ob = pool.back();
                pool.pop_back();
            }
            else
            {
                ob = create();
            }

		    return ob;
	    }

	    void recycle(T* ob)
	    {
		    ob->reset();
            pool.push_back(ob);
            maximumCreated = std::max(maximumCreated, pool.size());
	    }

        T* create()
        {
            if(factoryMethod)
                return factoryMethod();
            else
                return new T();
        }
    };
}
#endif