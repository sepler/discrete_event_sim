#include <stdio.h>
#include <stdlib.h>
#include "event.h"

typedef struct pqueue_node {
    struct sim_event* event;
    struct pqueue_node* before;
    struct pqueue_node* after;
} pqueue_node;

typedef struct pqueue {
    int size;
    struct pqueue_node *head;
} pqueue;

pqueue* create_p_queue() {
    pqueue* q;
    q = malloc(sizeof(pqueue));
    q->size = 0;
    q->head = NULL;
    return q;
}

int print_p_queue(pqueue* q) {
    if (q == NULL || q->size == 0) {
        printf("empty fifo\n");
        return 0;
    }
    pqueue_node* iter;
    iter = q->head;
    printf("%d,", iter->event->time);
    while (iter->after != NULL) {
        iter = iter->after;
        printf("%d,", iter->event->time);
    }
    printf("\n");
    return 1;
}

int push_p_queue(pqueue* q, sim_event* e) {
    if (q == NULL) {
        printf("fifo doesnt exist\n");
        return 0;
    }

    pqueue_node* node;
    node = malloc(sizeof(pqueue_node));
    node->after = NULL;
    node->before = NULL;
    node->event = e;

    if (q->size == 0) {
        q->head = node;
        q->size++;
        return 1;
    }

    if (node->event->time <= q->head->event->time) {
        node->after = q->head;
        q->head->before = node;
        q->head = node;
        q->size++;
        return 1;
    }

    pqueue_node* iter;
    iter = q->head;

    while (iter->after != NULL && node->event->time > iter->after->event->time) {
        iter = iter->after;
    }
    // val = 4
    // 3 6
    //iter->after = node;
    //node->before = iter;
    
    node->after = iter->after;
    node->before = iter;
    if (iter->after != NULL)
        iter->after->before = node;
    iter->after = node;
    
    q->size++;
    return 1;
}

sim_event* pop_p_queue(pqueue* q) {
    if (q == NULL || q->size < 1) {
        //printf("fifo doesnt exist\n");
        return NULL;
    }
    if (q->size == 1) {
        sim_event* e = q->head->event;
        //free(q->head);
        q->head = NULL;
        q->size--;
        return e;
    }
    sim_event* e = q->head->event;
    q->head = q->head->after;
    //free(q->head->before);
    q->head->before = NULL;
    q->size--;
    return e;
}

