#include "thread.h"
#include <sys/syscall.h>
#include <unistd.h>

static pid_t gettid();
static void *startThread(void *obj);

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;

    ThreadFunc threadFunc;
    pid_t *threadId;

    ThreadData(ThreadFunc func, pid_t *tid) : threadFunc(func), threadId(tid)
    {

    }

    void runInThread()
    {
        *threadId = gettid();
        threadId = NULL;
        threadFunc();
    }
};

static pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

static void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread::Thread(ThreadFunc func)
    : m_started(false),
      m_joined(false),
      m_tid(0),
      pthreadId(0),
      threadFunc(func)
{

}

Thread::~Thread()
{
    if (m_started && !m_joined)
    {
        pthread_detach(pthreadId);
    }
}

void Thread::start()
{
    if (!m_started)
    {
        ThreadData *data = new ThreadData(threadFunc, &m_tid);

        if (pthread_create(&pthreadId, NULL, &startThread, data) == 0)
        {
            usleep(100 * 1000);
            m_started = true;
        }
    }
}

void Thread::join()
{
    if (pthread_join(pthreadId, NULL) == 0)
    {
        m_joined = true;
    }
}

bool Thread::started() const
{
    return m_started;
}

bool Thread::joined() const
{
    return m_joined;
}

pid_t Thread::tid() const
{
    return m_tid;
}
