#include <vector>
#include "smartptr.h"
#include "thread.h"

template<typename T>
class thread_pool
{
public:
    thread_pool(size_t size)
        : size_(size)
    {
    }
    ~thread_pool()
    {
    }

    void execute()
    {
        for (size_t i = 0; i < size_; ++i)
        {
            smartptr<thread> element(new (std::nothrow)T());
            if (element != NULL)
            {
                thread_list_.push_back(element);
            }
        }

        std::vector<smartptr<thread> >::iterator it;
        for (it = thread_list_.begin(); it != thread_list_.end(); ++it)
        {
            (*it)->start();
        }
    }

    void wait()
    {
        std::vector<smartptr<thread> >::iterator it;
        for (it = thread_list_.begin(); it != thread_list_.end(); ++it)
        {
            (*it)->wait();
        }
    }
private:
    size_t size_;
    std::vector<smartptr<thread> > thread_list_;
};
