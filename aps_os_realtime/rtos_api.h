#ifndef APS_OS_REALTIME_RTOS_API_H
#define APS_OS_REALTIME_RTOS_API_H

#include "sys.h"

#define DeclareTask(TaskID, priority) \
TASK(TaskID); \
enum {TaskID##prior=priority}

#define TASK(TaskID) void TaskID(void)

#define DeclareEvent(task, EventID) \
TEvent Event##EventID = TEvent(task, 1 << (EventID - 1));

typedef void TTaskCall(void);

void ActivateTask(TTaskCall entry, int priority, char *name);
void TerminateTask(void);
int StartOS(TTaskCall entry, int priority, char *name);
void ShutdownOS();

int InitRes(const char *name);
void PIP_GetRes(int res);
void PIP_ReleaseRes(int res);

void SetEvent(char *name, TEvent event);
void GetEvent(int task, TEventMask *event);
void WaitEvent(TEvent event);
void ClearEvent();

void Dispatch();  // для вызова из других модулей
void Schedule(int task, int mode);



#endif //APS_OS_REALTIME_RTOS_API_H
