//Hersh Rudrawal
//cse13s spring 2021
#include "stack.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//Implemntation of the Stack ADT making use of nodes
//based off stack form asgn 4
struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack)); //create stack
    if (s) {
        s->top = 0; //set top
        s->capacity = capacity; //set capcity
        (s->items) = calloc(capacity, sizeof(Node *)); //create items
        if (!(s->items)) {
            free(s); //free and return null if fail
            s = NULL;
        }
    }
    return s;
}
void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items); //free items
        free(*s); //free stack
        *s = NULL;
    }
    return;
}
bool stack_empty(Stack *s) {
    return (s->top) == 0;
}
bool stack_full(Stack *s) {
    return (s->top) == (s->capacity);
}
uint32_t stack_size(Stack *s) {
    return (s->top);
}
bool stack_push(Stack *s, Node *n) {
    if (s) {
        if (stack_full(s)) { //check if full
            return false;
        }
        s->items[s->top] = n; //store n at top of stack
        s->top += 1; //increment top
        return true;
    }
    return false;
}
bool stack_pop(Stack *s, Node **n) {
    if (s) {
        if (stack_empty(s)) { //check if empty
            return false;
        }
        s->top -= 1; //top will now point to node
        *n = s->items[s->top];
        return true;
    }
    return false;
}
void stack_print(Stack *s) {
    printf("Stack\n");
    for (uint32_t i = 0; i < s->top; i += 1) {
        node_print(s->items[i]);
    }
}
