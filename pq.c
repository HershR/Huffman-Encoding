//Hersh Rudrawal
//CSE13s
#include "pq.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//Implementation of priority queue ADT making use of nodes
//uses insertion sort
struct PriorityQueue {
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    uint32_t capacity;
    Node **nodes;
};

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue)); //allocate memory
    if (q) {
        q->head = 0;
        q->tail = 0;
        q->size = 0;
        q->capacity = capacity;
        (q->nodes) = calloc(capacity, sizeof(Node *)); //create nodes
        if (!q->nodes) {
            free(q); //free and return null if nodes is not created
            q = NULL;
        }
    }
    return q;
}
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->nodes) {
        free((*q)->nodes); //free nodes
        free(*q); //free queue
        *q = NULL;
    }
    return;
}
//inspired by eugene's lab
bool pq_empty(PriorityQueue *q) {
    return q->size == 0;
}

bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (q) {
        if (pq_full(q)) {
            return false;
        }
        if (pq_empty(q)) {
            q->nodes[q->tail] = n;
            q->tail = ((q->tail + 1) % q->capacity); //ensure the tail wraps around
            q->size += 1;
            return true;
        }
        uint64_t i = q->tail;
        while (true) {
            if (i == q->head) {
                q->nodes[q->head] = n;
                break;
            } else if (n->frequency > q->nodes[(i + (q->capacity) - 1) % q->capacity]->frequency) {
                q->nodes[i] = n;
                break;
            }
            q->nodes[i] = q->nodes[(i + (q->capacity) - 1) % q->capacity];
            i = (i + (q->capacity) - 1) % q->capacity;
        }
        q->tail = ((q->tail + 1) % q->capacity); //ensure the tail wraps around
        q->size += 1;

        return true;
    }
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (q) {
        if (pq_empty(q)) {
            return false;
        }
        *n = q->nodes[q->head];
        q->size -= 1;
        q->head = ((q->head + 1) % q->capacity); //ensure the head wraps around
        return true;
    }
    return false;
}
void pq_print(PriorityQueue *q) {
    if (pq_empty(q)) {
        return;
    }
    uint32_t i = q->head;
    do {
        fprintf(stderr, "i: %d\t", i);
        node_print(q->nodes[i]);
        i = ((i + 1) % q->capacity);
    } while (i != q->tail);
}
