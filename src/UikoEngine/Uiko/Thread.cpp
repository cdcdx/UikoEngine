#ifdef __sun 
//
// Solaris 10 bug: it's supposed to be defined in pthread.h
//
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif
#endif

#include <climits>
#include <exception>
#include "thread.h"

#ifndef _WIN32
#include <unistd.h>
#endif

void thread::sleep(int waitdelay_ms)
{
#ifdef _WIN32
    Sleep((DWORD)waitdelay_ms);
#else
    usleep(waitdelay_ms * 1000);
#endif
}

#ifdef _WIN32

thread::thread()
: detached_(false), running_(false), handle_(INVALID_HANDLE_VALUE), id_(0)
{
}

thread::~thread()
{
    if (handle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
}

static unsigned int WINAPI thread_start_hook(void* arg)
{
    if (arg == 0)
        return 0;
    smartptr<thread> this_thread;
    try
    {
        this_thread.__assign(static_cast<thread*>(arg));
        this_thread->run();
    }
    catch(...)
    {
#if defined(_MSC_VER) && (_MSC_VER < 1300)
        terminate();
#else
        std::terminate();
#endif
    }

    this_thread->finished();

    return 0;
}

#include <process.h>

int thread::start(bool detach_flag, size_t stack_size)
{
    guard<mutex> g(state_lock_);

    if (running_)
    {
        return 0;
    }

    this->__incref();

    unsigned int id;
    handle_ = reinterpret_cast<HANDLE>(_beginthreadex(0, static_cast<unsigned int>(stack_size), thread_start_hook, this, 0, &id));
    id_ = id;

    if (handle_ == INVALID_HANDLE_VALUE)
    {
        this->__decref();
        return -1;
    }

    if (detach_flag)
    {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }

    detached_ = detach_flag;
    running_ = true;

    return 0;
}

void thread::stop()
{
    if (running_ && !detached_) 
    {
        TerminateThread(handle_, 0);

        //Closing a thread handle does not terminate the associated thread.
        //To remove a thread object, you must terminate the thread,
        //then close all handles to the thread.
        //The thread object remains in the system until
        //the thread has terminated and all handles to it have been
        //closed through a call to CloseHandle
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        detached_ = true;
    }
}

void thread::detach()
{
    if (running_ && !detached_)
    {
        if (handle_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
    }
    detached_ = true;
}

void thread::wait()
{
    if (running_ && !detached_)
    {
        if (handle_ != INVALID_HANDLE_VALUE)
        {
            WaitForSingleObject(handle_, INFINITE);
            // notice: don't add CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
        detached_ = true;
    }
}

void thread::finished()
{
    guard<mutex> g(state_lock_);
    running_ = false;

    if (handle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
    detached_ = true;

    // notice: don't add this->__decref();
}

#else

thread::thread()
: detached_(false), running_(false)
{
}

thread::~thread()
{
}

extern "C" 
{
    static void* thread_start_hook(void* arg)
    {
        if (arg == 0)
            return 0;

        smartptr<thread> this_thread;

        try
        {
            this_thread.__assign(static_cast<thread*>(arg));
            this_thread->run();
        }
        catch(...)
        {
            std::terminate();
        }

        this_thread->finished();

        return 0;
    }
}

int thread::start(bool detach_flag, size_t stack_size)
{
    guard<mutex> g(state_lock_);

    if (running_)
    {
        return 0;
    }

    __incref();

    pthread_attr_t attr;
    int rc = pthread_attr_init(&attr);
    if (rc != 0)
    {
        __decref();
        return -1;
    }

    if (detach_flag)
    {
        rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (rc != 0)
        {
            pthread_attr_destroy(&attr);
            __decref();
            return -1;
        }
    }

    if (stack_size > 0)
    {
        if (stack_size < PTHREAD_STACK_MIN)
        {
            stack_size = PTHREAD_STACK_MIN;
        }
#ifdef __APPLE__
        if (stack_size % 4096 > 0)
        {
            stack_size = stack_size / 4096 * 4096 + 4096;
        }
#endif
        rc = pthread_attr_setstacksize(&attr, stack_size);
        if (rc != 0)
        {
            pthread_attr_destroy(&attr);
            __decref();
            return -1;
        }
    }

    rc = pthread_create(&thread_, 0, thread_start_hook, this);
    if (rc != 0)
    {
        pthread_attr_destroy(&attr);
        __decref();
        return -1;
    }

    pthread_attr_destroy(&attr);

    detached_ = detach_flag;
    running_ = true;

    return 0;
}

void thread::stop()
{
    if (running_ && !detached_) 
    {
        pthread_cancel(thread_);
        pthread_detach(thread_);
        detached_ = true;
    }
}

void thread::detach()
{
    if (running_ && !detached_)
    {
        pthread_detach(thread_);
    }
    detached_ = true;
}

void thread::wait()
{
    if (running_ && !detached_)
    {
        pthread_join(thread_, 0);
        detached_ = true;
    }
}

void thread::finished()
{
    guard<mutex> g(state_lock_);
    running_ = false;
    detached_ = true;

    // notice: don't add this->__decref();
}

#endif

bool thread::operator==(const thread& rhs) const
{
    return this == &rhs;
}

bool thread::operator!=(const thread& rhs) const
{
    return this != &rhs;
}

bool thread::operator<(const thread& rhs) const
{
    return this < &rhs;
}

bool thread::is_running()
{
    guard<mutex> g(state_lock_);
    return running_;
}
