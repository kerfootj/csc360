#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct taskval taskval_t;
struct taskval {
    int        id;
    int        arrival_time;
    int        finish_time;
    float      cpu_request;
    float      cpu_used;
    taskval_t *next;
};

taskval_t *add_front(taskval_t *, taskval_t *);
taskval_t *add_end(taskval_t *, taskval_t *);
void       apply(taskval_t *, void(*fn)(taskval_t *, void *), void *arg);
void       end_task(taskval_t *);
taskval_t *new_task();
taskval_t *peek_front(taskval_t *);
taskval_t *remove_front(taskval_t *);

#endif
