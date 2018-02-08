// STEPHEN EPLER

#ifndef EVENT_H
#define EVENT_H

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

sim_event* create_event(event_t t, int id, int time);

#endif