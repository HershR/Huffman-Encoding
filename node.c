#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->symbol = symbol;
        n->frequency = frequency;
        n->left = NULL;
        n->right = NULL;
    }
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        free(*n); //free node
        *n = NULL;
    }
    return;
}

Node *node_join(Node *left, Node *right) {
    //inspired from sahiti's section
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->symbol = (uint8_t) '$';
        n->frequency = left->frequency + right->frequency;
        n->left = left;
        n->right = right;
    }
    return n;
}
void node_print(Node *n) {
    if (n) {
        fprintf(stderr, "Sym: %d Frq: %ld\n", n->symbol, n->frequency);
    }
}
