#ifndef __LOCKER_H__
#define __LOCKER_H__

#include <exception>
#include <pthread.h>
#include <semaphore.h>

// 信号量
class sem {
public:

    sem() {
        // sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));
        // sem为指向信号量结构的一个指针；
        // pshared不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享；
        // value给出了信号量的初始值。
        if(sem_init(&m_sem, 0, 0) != 0) {
            throw std::exception();
        }
    }

    sem(int num) {
        if(sem_init(&m_sem, 0, num) != 0) {
            throw std::exception();
        }
    }

    ~sem() {
        // 函数sem_destroy(sem_t *sem)用来释放信号量sem。　
        sem_destroy(&m_sem);
    }

    bool wait() {
        // 函数sem_wait( sem_t *sem )被用来阻塞当前线程直到信号量sem的值大于0，
        // 解除阻塞后将sem的值减一，表明公共资源经使用后减少。
        return sem_wait(&m_sem) == 0;
    }

    bool post() {
        // 函数sem_post( sem_t *sem )用来增加信号量的值。
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

// 互斥锁
class locker {
public:
    locker() {
        // pthread_mutex_init() 函数是以动态方式创建互斥锁的，
        // 参数attr指定了新建互斥锁的属性。如果参数attr为空(NULL)，则使用默认的互斥锁属性，默认属性为快速互斥锁 。
        if(pthread_mutex_init(&m_mutex, NULL) != 0) {
            throw std::exception();
        }
    }

    ~locker() {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock() {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock() {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    pthread_mutex_t* get() {
        return &m_mutex;
    }
private:
    pthread_mutex_t m_mutex;
};

// 条件变量
class cond {
public:
    cond() {
        if(pthread_cond_init(&m_cond, NULL) != 0) {
            throw std::exception();
        }
    }

    ~cond() {
        pthread_cond_destroy(&m_cond);
    }

    //条件变量的使用机制需要配合锁来使用
    //内部会有一次加锁和解锁
    //封装起来会使得更加简洁
    bool wait(pthread_mutex_t* m_mutex) {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool timewait(pthread_mutex_t* m_mutex, struct timespec t) {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timewait(&m_cond, m_mutex, &t);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    //  pthread_cond_signal函数的作用是发送一个信号给另外一个正在处于阻塞等待状态的线程,
    //  使其脱离阻塞状态,继续执行.如果没有线程处在阻塞等待状态,pthread_cond_signal也会成功返回。
    bool signal() {
        return pthread_cond_signal(&m_cond) == 0;
    }

    // 函数以广播的方式唤醒所有等待目标条件变量的线程
    bool broadcast() {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;
}
#endif