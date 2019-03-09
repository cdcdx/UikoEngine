#pragma once
#ifndef __mutex_h__
#define __mutex_h__

#ifdef _WIN32
#   include <windows.h>
#else
#   include <pthread.h>
#   include <errno.h>
#endif

//
// simple non-recursive mutex implementation.
//
class mutex
{
public:
    inline mutex();
    ~mutex();

    void lock() const;
    void unlock() const;

private:

    // noncopyable
    mutex(const mutex&);
    const mutex& operator=(const mutex&);

#ifdef _WIN32
    mutable CRITICAL_SECTION mutex_;
#else
    mutable pthread_mutex_t mutex_;
#endif
};

//
// simple recursive mutex implementation.
//
class recmutex
{
public:
    inline recmutex();
    ~recmutex();

    void lock() const;
    void unlock() const;

private:

    // noncopyable
    recmutex(const recmutex&);
    const recmutex& operator=(const recmutex&);

#ifdef _WIN32
    mutable CRITICAL_SECTION mutex_;
#else
    mutable pthread_mutex_t mutex_;
#endif
    mutable int count_;
};

template<typename T>
class guard
{
public:
    // = Initialization and termination methods.
    guard(T& m) : mutex_(m)
    {
        mutex_.lock();
    };
    ~guard()
    {
        mutex_.unlock();
    };
private:
    guard();
    // noncopyable
    guard(const guard&);
    const guard& operator=(const guard&);

    T& mutex_;
};

//
// for performance reasons the following functions are inlined.
//

#ifdef _WIN32

inline
mutex::mutex()
{
    InitializeCriticalSection(&mutex_);
}

inline
mutex::~mutex()
{
    DeleteCriticalSection(&mutex_);
}

inline void
mutex::lock() const
{
    EnterCriticalSection(&mutex_);
    // assert(mutex_.RecursionCount == 1);
}

inline void
mutex::unlock() const
{
    // assert(mutex_.RecursionCount == 1);
    LeaveCriticalSection(&mutex_);
}

#else

inline
mutex::mutex()
{
#ifdef NDEBUG
    pthread_mutex_init(&mutex_, 0);
#else

#if defined(__linux) && !defined(__USE_UNIX98)
    const pthread_mutexattr_t attr = { PTHREAD_MUTEX_ERRORCHECK_NP };
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
#endif
    pthread_mutex_init(&mutex_, &attr);

#if defined(__linux) && !defined(__USE_UNIX98)
    // nothing to do
#else
    pthread_mutexattr_destroy(&attr);
#endif
#endif
}

inline
mutex::~mutex()
{
    pthread_mutex_destroy(&mutex_);
}

inline void
mutex::lock() const
{
    pthread_mutex_lock(&mutex_);
}

inline void
mutex::unlock() const
{
    pthread_mutex_unlock(&mutex_);
}

#endif

//
// for performance reasons the following functions are inlined.
//

#ifdef _WIN32

inline
recmutex::recmutex() : count_(0)
{
    InitializeCriticalSection(&mutex_);
}

inline
recmutex::~recmutex()
{
    // assert(count_ == 0);
    DeleteCriticalSection(&mutex_);
}

inline void
recmutex::lock() const
{
    EnterCriticalSection(&mutex_);
    if (++count_ > 1)
    {
        LeaveCriticalSection(&mutex_);
    }
}

inline void
recmutex::unlock() const
{
    if (--count_ == 0)
    {
        LeaveCriticalSection(&mutex_);
    }
}

#else

inline
recmutex::recmutex() : count_(0)
{
#ifdef NDEBUG
    pthread_mutex_init(&mutex_, 0);
#else

#if defined(__linux) && !defined(__USE_UNIX98)
    const pthread_mutexattr_t attr = { PTHREAD_MUTEX_ERRORCHECK_NP };
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
    pthread_mutex_init(&mutex_, &attr);

#if defined(__linux) && !defined(__USE_UNIX98)
    // nothing to do
#else
    pthread_mutexattr_destroy(&attr);
#endif
#endif
}

inline
recmutex::~recmutex()
{
    // assert(count_ == 0);
    pthread_mutex_destroy(&mutex_);
}

inline void
recmutex::lock() const
{
    pthread_mutex_lock(&mutex_);
    if (++count_ > 1)
    {
        pthread_mutex_unlock(&mutex_);
    }
}

inline void
recmutex::unlock() const
{
    if (--count_ == 0)
    {
        pthread_mutex_unlock(&mutex_);
    }
}

#endif

#endif // __mutex_h__
