#ifndef __MYTHREADPOOL_H__
#define __MYTHREADPOOL_H__

#include "lib_use.h"
#include "myType.h"
#include "queue.h"

#define bool int

typedef struct ThreadPool{
    Queue *queue;
    pthread_t managerTid;
    pthread_t *tids;
    int threadNum;//tids数组长度，数组中值为0的位置为之前销毁的线程号所在的位置。
    pthread_mutex_t mutex_taskQueue;
    pthread_mutex_t mutex_threadPool;
    bool isExit;
    int threadNum_canceling;//需要销毁的线程数
    int threadNum_active;//存活的线程数
    int threadNum_busy;//执行任务的线程数
    int threadNum_min;//线程池最小存活线程数
    pthread_cond_t cond_queue_notEmpty;//队列不空
    pthread_cond_t cond_queue_notFull;

}ThreadPool;

typedef ThreadPool* pThreadPool;

pThreadPool threadPoolInit(int threadNum, int queueCapacity);
void addTask(pThreadPool pthpoll, Task task);
void threadPoolDestroy(pThreadPool *ppthpool);
void* threadsManager(void* arg);
//void* cancelFunc(void * arg);
void* threadFunc(void* arg);
void freeThreadPool(pThreadPool pthpool);
void expendThreadPool(pThreadPool pthpoll);
void threadPoolShrink(pThreadPool pthpool);

#endif