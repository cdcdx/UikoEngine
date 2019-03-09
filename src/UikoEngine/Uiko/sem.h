#pragma once
#ifndef __sem_h__
#define __sem_h__

#ifndef _WIN32
#   include <pthread.h>
#   include <semaphore.h>
#   include <errno.h>
#endif

class sem
{
public:
    inline sem();
    ~sem();

    bool timedwait(int timeout_ms);

    void post(int count = 1);

private:
    // noncopyable
    sem(const sem&);
    const sem& operator=(const sem&);

#ifdef _WIN32
    mutable HANDLE sem_;
#else
    sem_t sem_;
#endif
};

#ifdef _WIN32

inline sem::sem()
{
    sem_ = CreateSemaphore(0, 0, 0x7fffffff, 0);
}

inline sem::~sem()
{
    if (sem_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(sem_);
        sem_ = INVALID_HANDLE_VALUE;
    }
}

inline bool sem::timedwait(int timeout_ms)
{
    return WaitForSingleObject(sem_, static_cast<DWORD>(timeout_ms)) != WAIT_TIMEOUT;
}

inline void sem::post(int count)
{
    ReleaseSemaphore(sem_, count, 0);
}

#else

inline sem::sem()
{
    sem_init(&sem_, 0, 0);
}

inline sem::~sem()
{
    sem_destroy(&sem_);
}

inline bool sem::timedwait(int timeout_ms)
{
    struct timespec tm_diff;
    tm_diff.tv_sec  = timeout_ms / 1000;
    tm_diff.tv_nsec = (timeout_ms % 1000) * 1000 * 1000;

    struct timespec tm_spec;
    clock_gettime(CLOCK_REALTIME, &tm_spec);
    tm_spec.tv_sec  += tm_diff.tv_sec;
    tm_spec.tv_nsec += tm_diff.tv_nsec;

    if (tm_spec.tv_nsec > 1000 * 1000 * 1000)
    {
        tm_spec.tv_sec ++;
        tm_spec.tv_nsec -= 1000 * 1000 * 1000;
    }

    return sem_timedwait(&sem_, &tm_spec) > 0;
}

inline void sem::post(int count)
{
    sem_post(&sem_);
}

#endif

#endif // __sem_h__
