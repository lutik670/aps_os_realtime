#include <stdio.h>
#include <setjmp.h>
#include "sys.h"
#include "rtos_api.h"

// Инициализация задачи в системе
void ActivateTask(TTaskCall entry, int priority, char *name) {
    printf("[ActivateTask] %s\n", name);

    int occupy = FreeTask; // получаем номер свободной задачи
    FreeTask = TaskQueue[occupy].ref; // смещаем указатель на следующую свободную

    // Инициализация полей задачи
    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].events = 0;
    TaskQueue[occupy].waiting_events = 0;
    TaskQueue[occupy].entry = entry;
    TaskQueue[occupy].switchNumber = 0;
    TaskQueue[occupy].res = _NULL;
    TaskQueue[occupy].state = TASK_READY;

    printf("End of ActivateTask %s\n", name);

    Schedule(occupy, INSERT_TO_HEAD);
}

// Завершение задачи
void TerminateTask(void) {
    int task = RunningTask;
    printf("[Terminate Task] %s\n", TaskQueue[task].name);
    TaskQueue[task].state = TASK_DONE;
    RunningTask = _NULL;
    printf("End of Terminate Task %s\n", TaskQueue[task].name);
    Dispatch();
}

// Планировщик задач
void Schedule(int task, int mode) {
    printf("[Schedule] %s\n", TaskQueue[task].name);

    int cur = HeadTask;
    int prev = _NULL;
    int priority = TaskQueue[task].priority;

    while (cur != _NULL && TaskQueue[cur].priority > priority) {
        prev = cur;
        cur = TaskQueue[cur].ref;
    }

    if (mode == INSERT_TO_TAIL) {
        while (cur != _NULL && TaskQueue[cur].priority == priority) {
            prev = cur;
            cur = TaskQueue[cur].ref;
        }
    }

    TaskQueue[task].ref = cur;
    if (prev == _NULL) {
        HeadTask = task;
    } else {
        TaskQueue[prev].ref = task;
    }

    TaskQueue[task].state = TASK_READY;
    printf("End of Schedule %s\n", TaskQueue[task].name);

    Dispatch();
}

// Диспетчеризация задач
void Dispatch() {
    int task = HeadTask;
    while (task != _NULL) {
        switch (TaskQueue[task].state) {
            case TASK_DONE:
                task = TaskQueue[task].ref;
                continue;

            case TASK_RUNNING:
                printf("%s is running\n", TaskQueue[task].name);
                return;

            case TASK_READY:
                if (RunningTask != _NULL) {
                    task = TaskQueue[task].ref;
                    continue;
                }
                printf("%s started\n", TaskQueue[task].name);
                RunningTask = task;
                TaskQueue[task].state = TASK_RUNNING;

                if (TaskQueue[task].res != _NULL) {
                    PIP_GetRes(TaskQueue[task].res);
                }

                TaskQueue[task].switchNumber++;
                if (TaskQueue[task].switchNumber == 1) {
                    TaskQueue[task].entry();
                } else {
                    longjmp(TaskQueue[task].context, 1);
                }
                return;

            case TASK_WAITING:
                if ((TaskQueue[task].waiting_events & TaskQueue[task].events) ==
                    TaskQueue[task].waiting_events) {
                    TaskQueue[task].events &= ~TaskQueue[task].waiting_events;
                    TaskQueue[task].waiting_events = 0;
                    if ((TaskQueue[task].res != _NULL &&
                         ResourceQueue[TaskQueue[task].res].task == _NULL) ||
                        TaskQueue[task].res == _NULL) {
                        TaskQueue[task].state = TASK_READY;
                    }
                } else {
                    task = TaskQueue[task].ref;
                }
                continue;
        }
    }
}
