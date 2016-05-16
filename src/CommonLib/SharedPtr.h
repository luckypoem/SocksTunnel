#ifndef __H_SHAREDPTR__
#define __H_SHAREDPTR__

#include <stdint.h>
#include "Atomic.h"
#include <assert.h>
#if __cplusplus >= 201103L
#include <memory>
#endif
#ifndef NULL
#define NULL 0
#endif
#include <algorithm>

/*
 *   Using '#define NO_THREAD_SAFE' will get better efficiency.
 *   But in a multi-thread environment, it will crash...
 *   Known problems:
 *   Because of the static lock, this class may cause crash at then end of main func when it work together with a obj created by ISingleton
 *   However, if you use c++11, you will not necessary to consider this...
 */
#include "Mutex.h"

namespace ptr
{
#if __cplusplus < 201103L
    template<typename Object>
    class SharedPtr
    {
    private:
        template<typename T>
        class SharedPtrInner
        {
            friend class SharedPtr<T>;
        public:
            explicit SharedPtrInner(T *ptr = 0) : sptr_(ptr), counter_(1) {}
            ~SharedPtrInner() { delete sptr_; }

        public:
            Object *get() { return sptr_; }
            const Object *get() const { return sptr_; }

        public:
            void incRef() { ++counter_; }
            bool decRef() { --counter_; return counter_ == 0; }
            const uint32_t &getRefCount() const { return counter_; }

        private:
            T *sptr_;
            atom::Atomic<uint32_t>  counter_;
        };

        typedef SharedPtrInner<Object> Inner;
    public:
        explicit SharedPtr(Object *ptr = 0) : sptr_(new SharedPtrInner<Object>(ptr)) {}
        SharedPtr(const SharedPtr<Object> &sptr)
        {
            assert(sptr.sptr_);
            sptr.sptr_->incRef();
            sptr_ = sptr.sptr_;
        }

        ~SharedPtr()
        {
            {
#ifndef NO_THREAD_SAFE
                //gege wo fu le, shi zai xie bu chu bu yong mutex de xiancheng anquan sharedptr
                thread::mutex::ScopeLock<thread::mutex::Mutex> lock(mutex());
#endif
                assert(sptr_);
                if (sptr_->decRef())
                    delete sptr_;
            }
        }

    public:
        SharedPtr<Object> &operator=(const SharedPtr<Object> &sptr)
        {
            return reset(sptr);
        }

        SharedPtr<Object> &reset(const SharedPtr<Object> &sptr)
        {
            {
#ifndef NO_THREAD_SAFE
                thread::mutex::ScopeLock<thread::mutex::Mutex> lock(mutex());
#endif
                SharedPtr<Object> tmp(sptr);
                tmp.swap(*this);
            }
            return *this;
        }

        SharedPtr<Object> &reset(Object *ptr)
        {
            return reset(SharedPtr<Object>(ptr));
        }

        void swap(SharedPtr<Object> &other)
        {
            assert(sptr_);
            std::swap(sptr_, other.sptr_);
        }

        uint32_t getRefCount() const 
        {
            return sptr_->getRefCount();
        }

    public:
        Object *operator->() { return get(); }
        const Object *operator->() const { return get(); }
        Object *get() { return sptr_->get(); }
        const Object *get() const { return sptr_->get(); }

        Object &operator*() { return *get(); }
        const Object &operator*() const { return *get(); }

    protected:
        static thread::mutex::Mutex &mutex()
        {
            static thread::mutex::Mutex mu;
            return mu;
        }

    private:
        Inner *sptr_;
    };

    template <typename Object>
    void swap(SharedPtr<Object> &left, SharedPtr<Object> &right)
    {
        left.swap(right);
    }

#else
    template<typename Type>
    using SharedPtr = std::shared_ptr<Type>;
#endif

}

#endif

