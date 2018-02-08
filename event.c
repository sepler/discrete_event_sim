#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SIM_BEGIN,
    SIM_EXITS,
    JOB_ARRIVE,
    JOB_EXITS,
    JOB_STARTS_CPU,
    JOB_FINISH_CPU,
    JOB_STARTS_DISK1,
    JOB_FINISH_DISK1,
    JOB_STARTS_DISK2,
    JOB_FINISH_DISK2
} event_t;

typedef struct sim_event {
    event_t type;
    unsigned int id;
    unsigned int time; // priority
    unsigned int srv_arr;
} sim_event;


sim_event* create_event(event_t t, int id, int time) {
    sim_event* e;
    e = malloc(sizeof(sim_event));
    e->type = t;
    e->id = id;
    e->time = time;
    e->srv_arr = -1;
    return e;
}