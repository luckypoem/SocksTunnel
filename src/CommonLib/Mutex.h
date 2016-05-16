#ifndef __H_MUTEX__
#define __H_MUTEX__

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <assert.h>
#include <stdint.h>

namespace thread
{
    namespace mutex
    {
        class Mutex;
        template<typename LockableObject> class ScopeLock;

        template<typename LockableObject>
        class ScopeLock
        {
        public:
            ScopeLock(LockableObject &lock);
            ~ScopeLock();

        private:
            LockableObject &lock_;
        };
        class Mutex
        {
        public:
#ifdef _WIN32
            typedef CRITICAL_SECTION MutexID;
#else
            typedef pthread_mutex_t MutexID;
#endif
            Mutex();
            ~Mutex();

        public:
            void lock();
            void unlock();
            bool tryLock();
            MutexID &getMutexID() { return mid_; }

        protected:
            void setMutexID(const MutexID &id) { getMutexID() = id; }

        private:
            MutexID mid_;
        };

        template<typename LockableObject>
        inline ScopeLock<LockableObject>::ScopeLock(LockableObject &lock) : lock_(lock)
        {
            lock_.lock();
        }

        template<typename LockableObject>
        inline ScopeLock<LockableObject>::~ScopeLock()
        {
            lock_.unlock();
        }

        class RwLock
        {
#ifdef _WIN32
            typedef SRWLOCK RwLockID;
#else
            typedef pthread_rwlock_t RwLockID;
#endif
        public:
            RwLock();
            ~RwLock();

        public:
            void readLock();
            void readUnlock();

            void writeLock();
            void writeUnlock();

        protected:
            RwLockID &getRwLockID() { return rwId_; }

        protected:
            RwLock(const RwLock &) {}
            RwLock &operator=(const RwLock &) { return *this; }

        private:
            RwLockID rwId_;
        };
    }
    namespace barrier
    {
        class Barrier
        {
#ifdef _WIN32
            typedef SYNCHRONIZATION_BARRIER BarrierID;
#else
            typedef pthread_barrier_t BarrierID;
#endif
        public:
            Barrier(uint32_t count);
            ~Barrier();

        public:
            void wait();

        protected:
            BarrierID &getBarrierID() { return bid_; }

        private:
            BarrierID bid_;
        };
    }
}

#endif

