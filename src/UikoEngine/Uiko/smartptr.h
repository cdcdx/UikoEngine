#pragma once
#ifndef __smartptr_h__
#define __smartptr_h__

#if (defined(__linux) || defined(__FreeBSD__)) && (defined(__i386) || defined(__x86_64)) && !defined(__ICC)
#   ifndef USE_ATOMIC_LONG
#       define USE_ATOMIC_LONG
#   endif
// __ICC: The inline assembler causes problems with shared libraries.
//
// Linux only. Unfortunately, asm/atomic.h builds non-SMP safe code
// with non-SMP kernels. This means that executables compiled with a
// non-SMP kernel would fail randomly due to concurrency errors with
// reference counting on SMP hosts. Therefore the relevent pieces of
// atomic.h are more-or-less duplicated.
//
#elif defined(_WIN32)
#   include <windows.h>
#else
// No include
#endif

#include "mutex.h"
#if defined(USE_ATOMIC_LONG)
#include "atomic.h"
#endif

//
// Base classes for reference counted types. The smartptr template
// can be used for smart pointers to types derived from these bases.
//
// shared_object
// ===============
//
// A thread-safe base class for reference-counted types.
//

class shared_object
{
public:
    shared_object() : value_(0), no_delete_(false)
    {
    }

    virtual ~shared_object()
    {
    }

    void __incref()
    {
#if defined(_WIN32)
        ::InterlockedIncrement(const_cast<long *>(&(this->value_)));
#elif defined(USE_ATOMIC_LONG)
        ++(this->value_);
#else
        mutex_.lock();
        ++(this->value_);
        mutex_.unlock();
#endif
    }

    void __decref(bool no_delete = false)
    {
        // notice: this need 'value' stack variable
#if defined(_WIN32)
        long value = ::InterlockedDecrement(const_cast<long *>(&(this->value_)));
#elif defined(USE_ATOMIC_LONG)
        long value = --(this->value_);
#else
        mutex_.lock();
        long value = --(this->value_);
        mutex_.unlock();
#endif
        if (value == 0 && !no_delete && !no_delete_)
        {
            no_delete_ = true;
            delete this;
        }
    }

    int __getref()
    {
#if defined(_WIN32)
        return ::InterlockedExchangeAdd(const_cast<long *>(&(this->value_)), 0);
#elif defined(USE_ATOMIC_LONG)
        return this->value_.value();
#else
        mutex_.lock();
        int value = (this->value_);
        mutex_.unlock();
        return value;
#endif
    }

private:
    shared_object(const shared_object&);
    const shared_object& operator=(const shared_object&);

    // Current object decorated by the atomic op.
#if defined(_WIN32)
    volatile long value_;
#elif defined(USE_ATOMIC_LONG)
    atomic_long value_;
#else
    volatile long value_;
    mutex mutex_;
#endif
    bool no_delete_;
};


// class_object
// ===============
//
// A thread-safe base class for class object.
//
class class_object : public shared_object
{
public:
    class_object() : name_("") {};
    class_object(const char* name) : name_(name) {};
    virtual ~class_object() { name_ = NULL; };

    const char* __getname() const { return name_; };

private:
    const char* name_;
};


//
// "smart pointer" class for classes derived from shared_object.
//
template<typename T_Class, typename endclass = shared_object, typename T_Lock = mutex>
class smartptr
{
public:
    explicit smartptr(T_Class* handler = 0)
    {
        if (handler)
        {
            handler->endclass::__incref();
        }
        this->handler_ = handler;
    }

    smartptr(const smartptr& right)
    {
        this->handler_ = (const_cast<smartptr&>(right)).__lock_incref();
    }

    template<typename Y>
    smartptr(const smartptr<Y>& right)
    {
        this->handler_ = (const_cast<smartptr<Y>&>(right)).__lock_incref();
    }

    smartptr& operator=(T_Class* right_ptr)
    {   
        // assign compatible right (assume pointer)
        if (this->handler_ != right_ptr)
        {
            if (right_ptr)
            {
                right_ptr->endclass::__incref();
            }

            this->lock_.lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if (ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.unlock();
        }
        return (*this);
    }

    smartptr& operator=(const smartptr& right)
    {   // assign compatible right (assume pointer)
        if (this->handler_ != right.handler_)
        {
            T_Class* right_ptr = (const_cast<smartptr&>(right)).__lock_incref();

            this->lock_.lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if (ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.unlock();
        }
        return (*this);
    }

    template<typename Y>
    smartptr& operator=(const smartptr<Y>& right)
    {
        if(this->handler_ != right.handler_)
        {
            T_Class* right_ptr = (const_cast<smartptr<Y>&>(right)).__lock_incref();

            this->lock_.lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if(ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.unlock();
        }
        return *this;
    }

    ~smartptr()
    {
        if (this->handler_)
            this->handler_->endclass::__decref();
    }

    void __assign(T_Class* right)
    {
        this->handler_ = right;
    }

    bool __empty() const
    {
        return (this->handler_ == 0);
    }

    bool operator==(const smartptr& right) const
    {
        return (this->handler_ == right.handler_);
    }
    bool operator!=(const smartptr& right) const
    {
        return (this->handler_ != right.handler_);
    }
    bool operator==(T_Class* right) const
    {
        return (this->handler_ == right);
    }
    bool operator!=(T_Class* right) const
    {
        return (this->handler_ != right);
    }
    T_Class& operator*() const
    {
        return (*(this->handler_));
    }
    T_Class *operator->() const
    {
        return this->handler_;
    }
    T_Class* get() const
    {
        return this->handler_;
    }
    operator T_Class*() const
    {
        return this->handler_;
    }

    template<typename Y>
    static smartptr __dynamic_cast(const smartptr<Y>& right)
    {
        return smartptr(dynamic_cast<T_Class* const>(right.handler_));
    }

    template<typename Y>
    static smartptr __dynamic_cast(Y* right_ptr)
    {
        return smartptr(dynamic_cast<T_Class*>(right_ptr));
    }

    // dynamic cast need handler_ and __incref is public
    T_Class* __lock_incref() 
    {
        this->lock_.lock();
        if (this->handler_)
            this->handler_->endclass::__incref();
        this->lock_.unlock();
        return this->handler_;
    }

    T_Class* handler_;

private:
    T_Lock lock_;

};

template<class T> class scopedptr // noncopyable
{
public:
    typedef T element_type;
    typedef scopedptr<T> this_type;

    explicit scopedptr(T * p = 0) : ptr(p) // never throws
    {
    }

    ~scopedptr() // never throws
    {
        checked_delete(ptr);
    }

    void reset(T * p = 0) // never throws
    {
        this_type(p).swap(*this);
    }

    T & operator*() const // never throws
    {
        return *ptr;
    }

    T * operator->() const // never throws
    {
        return ptr;
    }

    T * get() const // never throws
    {
        return ptr;
    }

    // implicit conversion to "bool"

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return ptr == 0 ? 0 : &this_type::ptr;
    }

    bool operator!() const // never throws
    {
        return ptr == 0;
    }

    void swap(scopedptr & b) // never throws
    {
        T* tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }

private:
    scopedptr(scopedptr const&);
    const scopedptr& operator=(scopedptr const&);

    void operator==(scopedptr const&) const;
    void operator!=(scopedptr const&) const;

    inline void checked_delete(T * x)
    {
        // intentionally complex - simplification causes regressions
        typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
        (void) sizeof(type_must_be_complete);
        delete x;
    }

    T* ptr;
};

#endif // __smartptr_h__
