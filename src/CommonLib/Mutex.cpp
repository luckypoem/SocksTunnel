#include "Mutex.h"

namespace thread
{
    namespace mutex
    {
        Mutex::Mutex()
        {
#ifdef _WIN32
            InitializeCriticalSection(&getMutexID());
            //setMutexID(CreateMutex(NULL, FALSE, NULL));
#else
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#ifdef PROCESS_MUTEX
            pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
#endif
            pthread_mutex_init(&getMutexID(), &attr);
            pthread_mutexattr_destroy(&attr);
#endif
        }

        Mutex::~Mutex()
        {
#ifdef _WIN32
            DeleteCriticalSection(&getMutexID());
            //CloseHandle(getMutexID());
#else
            pthread_mutex_destroy(&getMutexID());
#endif
        }

        void Mutex::lock()
        {
#ifdef _WIN32
            //WaitForSingleObject(getMutexID(), INFINITE);
            EnterCriticalSection(&getMutexID());
#else
            bool ret = pthread_mutex_lock(&getMutexID());
            assert(ret == 0);
#endif
        }

        void Mutex::unlock()
        {
#ifdef _WIN32
            LeaveCriticalSection(&getMutexID());
            //ReleaseMutex(getMutexID());
#else
            bool ret = pthread_mutex_unlock(&getMutexID());
            //assert(ret == 0);
#endif
        }

        bool Mutex::tryLock()
        {
#ifdef _WIN32
            return TryEnterCriticalSection(&getMutexID()) != 0;
            //return WaitForSingleObject(getMutexID(), 0) == 0x00000000L;
#else
            return pthread_mutex_trylock(&getMutexID()) == 0;
#endif
            return false;
        }
        RwLock::RwLock()
        {
#ifdef _WIN32
            InitializeSRWLock(&getRwLockID());
#else
            pthread_rwlock_init(&getRwLockID(), NULL);
#endif
        }
        RwLock::~RwLock()
        {
#ifdef _WIN32

#else
            pthread_rwlock_destroy(&getRwLockID());
#endif
        }
        void RwLock::readLock()
        {
#ifdef _WIN32
            AcquireSRWLockShared(&getRwLockID());
#else
            pthread_rwlock_rdlock(&getRwLockID());
#endif
        }

        void RwLock::readUnlock()
        {
#ifdef _WIN32
            ReleaseSRWLockShared(&getRwLockID());
#else
            pthread_rwlock_unlock(&getRwLockID());
#endif
        }

        void RwLock::writeLock()
        {
#ifdef _WIN32
            AcquireSRWLockExclusive(&getRwLockID());
#else
            pthread_rwlock_wrlock(&getRwLockID());
#endif
        }

        void RwLock::writeUnlock()
        {
#ifdef _WIN32
            ReleaseSRWLockExclusive(&getRwLockID());
#else
            pthread_rwlock_unlock(&getRwLockID());
#endif
        }
    }

    namespace barrier
    {
        Barrier::Barrier(uint32_t count)
        {
#ifdef _WIN32
            InitializeSynchronizationBarrier(&getBarrierID(), count, -1);
#else
            pthread_barrier_init(&getBarrierID(), NULL, count);
#endif
        }
        Barrier::~Barrier()
        {
#ifdef _WIN32
            DeleteSynchronizationBarrier(&getBarrierID());
#else
            pthread_barrier_destroy(&getBarrierID());
#endif
        }

        void Barrier::wait()
        {
#ifdef _WIN32
            EnterSynchronizationBarrier(&getBarrierID(), SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);
#else
            pthread_barrier_wait(&getBarrierID());
#endif
        }
    }
}
