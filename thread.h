#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <functional>

class Thread
{
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc func);
    ~Thread();
    void start();
    void join();
    bool started() const;
    bool joined() const;
    pid_t tid() const;
private:
    Thread(const Thread &) = delete;
    Thread &operator = (const Thread &) = delete;

    bool       m_started;
    bool       m_joined;
    pid_t      m_tid;
    pthread_t  pthreadId;
    ThreadFunc threadFunc;
};

#endif
