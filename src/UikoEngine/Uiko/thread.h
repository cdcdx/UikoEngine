#pragma once
#ifndef __thread_h__
#define __thread_h__

#include "smartptr.h"
#include "sem.h"

class thread : public shared_object
{
public:
    thread();
    virtual ~thread();

    virtual void run() = 0;
    virtual void terminate() = 0;

    static void sleep(int waitdelay_ms);

    // if detach_flag == true, don't use wait()
    int start(bool detach_flag = false, size_t stack_size = 0);
    void stop();
    void detach();
    void wait();

    bool operator==(const thread&) const;
    bool operator!=(const thread&) const;
    bool operator<(const thread&) const;

    bool is_running();
    void finished();

protected:
    mutex state_lock_;
    bool detached_;
    bool running_;

#ifdef _WIN32
    HANDLE handle_;
    DWORD id_;
#else
    pthread_t thread_;
#endif

private:
    thread(const thread&);         // Copying is forbidden
    const thread& operator=(const thread&);      // Assignment is forbidden
};

#endif // __thread_h__
