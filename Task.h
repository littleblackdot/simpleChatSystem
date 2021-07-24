#ifndef __TASK_H__
#define __TASK_H__

typedef void (*taskFuncType)(void *);
typedef struct Task{
	taskFuncType taskFunc;
	void *arg;
}Task;


#endif // __