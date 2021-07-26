#include "myThreadPool.h"

void* threadFunc(void* arg){
    pThreadPool pthpool = (pThreadPool)arg; 
    while(1){ 
        //printf("%d: 1\n", pthread_self());
        pthread_mutex_lock(&pthpool->mutex_taskQueue);
        while(isEmpty(pthpool->queue)){
            if(pthpool->isExit){//销毁线程池开始且任务队列为空就退出
                pthread_mutex_unlock(&pthpool->mutex_taskQueue);
                break;
            }
            //printf("%d: 2\n", pthread_self());
            pthread_cond_wait(&pthpool->cond_queue_notEmpty, &pthpool->mutex_taskQueue);

            pthread_mutex_lock(&pthpool->mutex_threadPool);
            //printf("%d: 3\n", pthread_self());

            if(pthpool->threadNum_canceling > 0){
                (pthpool->threadNum_canceling)--;
                if(pthpool->threadNum_active > pthpool->threadNum_min){
                    for(int i=0; i<pthpool->threadNum; i++){
                        if(pthpool->tids[i] == pthread_self()){ 
                            pthpool->tids[i] = 0;
                        }   
                    }
                    (pthpool->threadNum_active)--;
                    pthread_mutex_unlock(&pthpool->mutex_taskQueue);
                    pthread_mutex_unlock(&pthpool->mutex_threadPool); 
                    if(pthread_detach(pthread_self()) != 0){
                        printf("detach thread: %ld fail\n", pthread_self());
                    }
                    pthread_exit(NULL);
                }    
            }

            pthread_mutex_unlock(&pthpool->mutex_threadPool); 

        }
        pthread_mutex_lock(&pthpool->mutex_threadPool);
        //printf("%d: 4\n", pthread_self());

        if(pthpool->isExit && isEmpty(pthpool->queue)){//线程池销毁开始且任务队列为空就退出线程
            if(pthread_detach(pthread_self()) != 0){
                printf("detach thread: %ld fail\n", pthread_self());
            }
            (pthpool->threadNum_active)--;
            pthread_mutex_unlock(&pthpool->mutex_threadPool);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&pthpool->mutex_threadPool);

        pthread_cond_signal(&pthpool->cond_queue_notFull);
        Task task = deleteItem(pthpool->queue);
        pthread_mutex_unlock(&pthpool->mutex_taskQueue);

        pthread_mutex_lock(&pthpool->mutex_threadPool);
        //printf("%d: 5\n", pthread_self());

        (pthpool->threadNum_busy)++;
        pthread_mutex_unlock(&pthpool->mutex_threadPool); 

        task.taskFunc(task.arg1, task.arg2, task.arg3, task.arg4);

        pthread_mutex_lock(&pthpool->mutex_threadPool);
        //printf("%d: 6\n", pthread_self());
        (pthpool->threadNum_busy)--;
        pthread_mutex_unlock(&pthpool->mutex_threadPool);               
    }

}

pThreadPool threadPoolInit(int threadNum, int queueCapacity){
    pThreadPool pthpool = (pThreadPool)malloc(sizeof(ThreadPool));
    pthpool->queue = createQueue(queueCapacity);
    pthpool->tids = (pthread_t *)malloc(sizeof(pthread_t) * threadNum);
    pthpool->threadNum = threadNum;
    pthpool->threadNum_active = threadNum;
    pthpool->threadNum_busy = 0;
    pthpool->threadNum_canceling = 0;
    pthpool->threadNum_min = threadNum/2;
    pthpool->isExit = 0;
    pthread_mutex_init(&pthpool->mutex_taskQueue, NULL);
    pthread_mutex_init(&pthpool->mutex_threadPool, NULL);
    pthread_cond_init(&pthpool->cond_queue_notEmpty, NULL);
    pthread_cond_init(&pthpool->cond_queue_notFull, NULL);

    for(int i = 0; i < threadNum; i++){
        if(pthread_create((pthpool->tids)+i, NULL, threadFunc, (void*)pthpool) != 0){
            perror("pthread_create failed");
        }
    }
    if(pthread_create(&pthpool->managerTid, NULL, threadsManager, (void*)pthpool) != 0){
            perror("pthread_create failed");
    }
    return pthpool;
}

void addTask(pThreadPool pthpool, Task task){
    if(pthpool == NULL){
        printf("pthpool is null");
        return ;
    }
    if(pthpool->isExit){
        printf("threadPool exiting, ignore the task added\n");
        return;   
    }
    pthread_mutex_lock(&pthpool->mutex_taskQueue);
    while(isFull(pthpool->queue)){
        pthread_cond_wait(&pthpool->cond_queue_notFull, &pthpool->mutex_taskQueue);
    }
    pthread_cond_signal(&pthpool->cond_queue_notEmpty);
    addItem(pthpool->queue, task);
    pthread_mutex_unlock(&pthpool->mutex_taskQueue);
}

void* threadsManager(void* arg){
    pThreadPool pthpool = (pThreadPool)arg;
    while(1){
        if(pthpool->isExit){
            pthread_exit(NULL);
        }
        sleep(2);
        pthread_mutex_lock(&pthpool->mutex_taskQueue);
        pthread_mutex_lock(&pthpool->mutex_threadPool);
        if(pthpool->threadNum_busy == pthpool->threadNum_active && !isEmpty(pthpool->queue)){//任务队列到达容量2/3就把线程数扩容
            expendThreadPool(pthpool);
        }
        pthread_mutex_unlock(&pthpool->mutex_threadPool);
        pthread_mutex_unlock(&pthpool->mutex_taskQueue);

        pthread_mutex_lock(&pthpool->mutex_threadPool);
        if(pthpool->threadNum_busy*2 < pthpool->threadNum_active){//繁忙线程数比活跃线程数的一般还少则缩水线程池
            threadPoolShrink(pthpool);
        }
        pthread_mutex_unlock(&pthpool->mutex_threadPool);
        
    }
}

void threadPoolShrink(pThreadPool pthpool){ 
    pthpool->threadNum_canceling = 10;
    pthread_cond_broadcast(&pthpool->cond_queue_notEmpty);
}


void expendThreadPool(pThreadPool pthpool){
    int curThreadNum;
    int targetThreadNum;
    curThreadNum = pthpool->threadNum_active;
    targetThreadNum = curThreadNum * 1.5;//活跃线程扩容1.5倍
    for(int i=0; i<pthpool->threadNum && pthpool->threadNum_active<targetThreadNum; i++){
        if(pthpool->tids[i] == 0){
            (pthpool->threadNum_active)++;
            pthread_create((pthpool->tids)+i, NULL, threadFunc, (void *)pthpool);
        }
    }
    if(pthpool->threadNum_active < targetThreadNum){
        int temp = targetThreadNum - pthpool->threadNum;
        pthread_t *tids = (pthread_t*)malloc(sizeof(pthread_t) * (pthpool->threadNum+temp));
        for(int i = 0; i < pthpool->threadNum; i++){
            tids[i] = pthpool->tids[i];
        }
        for(int i=pthpool->threadNum; i<targetThreadNum; i++){
            if(pthread_create(tids+i, NULL, threadFunc, (void*)pthpool) != 0){
                perror("pthread_create failed");
            }
            (pthpool->threadNum_active)++;
        }
        free(pthpool->tids);
        pthpool->tids = tids;
        pthpool->threadNum = targetThreadNum;
        pthpool->threadNum_active = targetThreadNum;
    }
}

void threadPoolDestroy(pThreadPool *ppthpool){
    pThreadPool pthpool =*ppthpool;

    pthread_mutex_lock(&pthpool->mutex_threadPool);
    pthpool->isExit = 1;//设置线程销毁信号
    pthread_mutex_unlock(&pthpool->mutex_threadPool);

    pthread_join(pthpool->managerTid, NULL);

    pthread_cond_broadcast(&pthpool->cond_queue_notEmpty);//可能有线程阻塞在等待队列非空条件（cond_queue_notEmpty）而接受不到线程池销毁的信号
    while(pthpool->threadNum_active > 0){
        sleep(1);
    }
    destoryQueue(&pthpool->queue);
    free(pthpool->tids);
    pthread_mutex_destroy(&pthpool->mutex_taskQueue);
    pthread_mutex_destroy(&pthpool->mutex_threadPool);
    pthread_cond_destroy(&pthpool->cond_queue_notEmpty);
    pthread_cond_destroy(&pthpool->cond_queue_notFull);
    
    free(pthpool);
    *ppthpool = NULL;
}

