#include <stdio.h>
#include <stdlib.h>
#include "event.h"


typedef struct queue {
    int size;
    struct queue_node *head;
} queue;

typedef struct queue_node {
    struct sim_event* event;
    struct queue_node* before;
    struct queue_node* after;
} queue_node;


queue* create_queue() {
    queue* q;
    q = malloc(sizeof(queue));
    q->size = 0;
    q->head = NULL;
    return q;
}

int print_queue(queue* q) {
    if (q == NULL || q->size == 0) {
        printf("empty fifo\n");
        return 0;
    }
    queue_node* iter;
    iter = q->head;
    printf("%d,", iter->event->time);
    while (iter->after != NULL) {
        iter = iter->after;
        printf("%d,", iter->event->time);
    }
    printf("\n");
    return 1;
}

int push_queue(queue* q, sim_event* e) {
    if (q == NULL) {
        printf("fifo doesnt exist\n");
        return 0;
    }
    queue_node* node;
    node = malloc(sizeof(queue_node));
    node->after = NULL;
    node->before = NULL;
    node->event = e;

    if (q->size == 0) {
        q->head = node;
        q->size++;
        return 1;
    }

    queue_node* iter;
    iter = q->head;
    while (iter->after != NULL) {
        iter = iter->after;
    }
    iter->after = node;
    node->before = iter;
    q->size++;
    return 1;
}

sim_event* pop_queue(queue* q) {
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

sim_event peek_queue(queue* q) {
    if (q == NULL || q->size < 1) {
        printf("fifo doesnt exist\n");
        sim_event e;
        e.type = -1;
        return e;
    }
    sim_event e = *q->head->event;
    return e;
}