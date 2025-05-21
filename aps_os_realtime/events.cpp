#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "rtos_api.h"
#include <exception>

void SetEvent(char *name, TEvent event) {
    int task = 0;
    for (; task < MAX_TASK; task++) {
        if (strcmp(TaskQueue[task].name, event.task) == 0)
            break;
    }

    if (strcmp(TaskQueue[task].name, event.task) != 0) {
        printf("Can not set event %i to task %s", event.mask, TaskQueue[task].name);
        throw std::exception();
    }

    printf("Set %i event to %s\n", event.mask, TaskQueue[task].name);
    TaskQueue[task].events = event.mask;

    Dispatch();
}

void WaitEvent(TEvent event) {
    printf("%s is waiting event %i\n", TaskQueue[RunningTask].name, event.mask);
    TaskQueue[RunningTask].state = TASK_WAITING;
    TaskQueue[RunningTask].waiting_events |= event.mask;

    int task = RunningTask;
    RunningTask = _NULL;
    setjmp(TaskQueue[task].context);

    Dispatch();
}

void ClearEvent() {
    TaskQueue[RunningTask].events = 0;
}
