// STEPHEN EPLER

#ifndef PQUEUE_H
#define PQUEUE_H
#ifndef EVENT_H
#include "event.h"
#endif

typedef struct pqueue_node {
    struct sim_event* event;
    struct pqueue_node* before;
    struct pqueue_node* after;
} pqueue_node;

typedef struct pqueue {
    int size;
    struct pqueue_node* head;
} pqueue;


pqueue* create_p_queue();
int print_p_queue( pqueue* root );
int push_p_queue( pqueue* root, sim_event* e );
sim_event* pop_p_queue( pqueue* root );
sim_event peek_p_queue( pqueue* root );

#endif