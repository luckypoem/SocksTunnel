#ifndef __H_ATOMIC__
#define __H_ATOMIC__

#if __cplusplus >= 201103L
#include <atomic>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace atom
{
#if __cplusplus >= 201103L
    template<typename Type>
    using Atomic = std::atomic<Type>;
#else
    //pool implement...
    template<typename Object>
    class Atomic
    {
    public:
        Atomic(Object obj = 0) :obj_(static_cast<long>(obj)) {}
        ~Atomic() {}

        Atomic<Object> &operator=(const Object &obj);
        Atomic<Object> &operator=(const Atomic<Object> &obj);
        Atomic<Object> &operator++();
        Atomic<Object> &operator--();
        Atomic<Object> &operator-=(const Object &obj);
        Atomic<Object> &operator-=(const Atomic<Object> &obj);
        Atomic<Object> &operator+=(const Object &obj);
        Atomic<Object> &operator+=(const Atomic<Object> &obj);
        operator Object() const
        {
            return static_cast<Object>(obj_);
        }

    protected:
        Atomic(Atomic &) {}

    private:
        long obj_;
    };

    template<typename Object>
    inline Atomic<Object> &Atomic<Object>::operator=(const Object &obj)
    {
        obj_ = obj;
        return *this;
    }

    template<typename Object>
    inline Atomic<Object> &Atomic<Object>::operator=(const Atomic<Object> & obj)
    {
        obj_ = obj.obj_;
        return *this;
    }

    template<typename Object>
    inline Atomic<Object> &Atomic<Object>::operator++()
    {
        //thread::mutex::ScopeLock<thread::mutex::Mutex> lock(getMutex());
        //++obj_;
#ifdef _WIN32
        InterlockedIncrement(&obj_);
#else
        __sync_add_and_fetch(&obj_, 1);
#endif
        return *this;
    }

    template<typename Object>
    inline Atomic<Object> &Atomic<Object>::operator--()
    {
#ifdef _WIN32
        InterlockedDecrement(&obj_);
#else
        __sync_sub_and_fetch(&obj_, 1);
#endif
        return *this;
    }
    template<typename Object>
    inline Atomic<Object>& Atomic<Object>::operator-=(const Object & obj)
    {
#ifdef _WIN32
        InterlockedAdd(&obj_, obj * (-1));
#else
        __sync_sub_and_fetch(&obj_, obj);
#endif
        return *this;
    }
    template<typename Object>
    inline Atomic<Object>& Atomic<Object>::operator-=(const Atomic<Object>& obj)
    {
#ifdef _WIN32
        InterlockedAdd(&obj_, obj.obj_ * (-1));
#else
        __sync_sub_and_fetch(&obj_, obj.obj_);
#endif
        return *this;
    }
    template<typename Object>
    inline Atomic<Object>& Atomic<Object>::operator+=(const Object & obj)
    {
#ifdef _WIN32
        InterlockedAdd(&obj_, obj);
#else
        __sync_add_and_fetch(&obj_, obj);
#endif
        return *this;
    }
    template<typename Object>
    inline Atomic<Object>& Atomic<Object>::operator+=(const Atomic<Object>& obj)
    {
#ifdef _WIN32
        InterlockedAdd(&obj_, obj.obj_);
#else
        __sync_add_and_fetch(&obj_, obj.obj_);
#endif
        return *this;
    }
#endif
}

#endif
