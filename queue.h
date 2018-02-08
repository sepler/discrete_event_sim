// STEPHEN EPLER

#ifndef QUEUE_H
#define QUEUE_H
#ifndef EVENT_H
#include "event.h"
#endif

typedef struct queue {
    int size;
    struct queue_node* head;
} queue;

queue* create_queue();
int print_queue( queue* root );
int push_queue( queue* root, sim_event* event );
sim_event* pop_queue( queue* root );
sim_event peek_queue( queue* root );

#endif