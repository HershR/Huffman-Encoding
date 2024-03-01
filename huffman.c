//Hersh Rudrawal
//CSE 13s Sprint 2021
#include "huffman.h"

#include "code.h"
#include "defines.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//This file contanes funtions that do the following
//Build a node tree given a array
//Build codes for a tree given the tree using post order traversal
//Rebuild a tree given the tree dump
//And delete a tree
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *q = pq_create(ALPHABET); //create priority queue to store and sort nodes
    for (uint32_t i = 0; i < ALPHABET; i++) { //iterate historgram
        //create nodes out of non zero entries and enqueue them
        if (hist[i] > 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(q, n);
        }
    }
    //create tree by joining the lowest frequecy nodes
    while (pq_size(q) > 1) {
        Node *l; //left child
        Node *r; //right child
        dequeue(q, &l); //dequeue left node
        dequeue(q, &r); //dequeue right node
        Node *p = node_join(l, r); //join to form parent node
        enqueue(q, p); //enqueue parent node
    }
    Node *t;
    dequeue(q, &t); //tree root
    pq_delete(&q); //delete pq
    return t;
}
//funtion to conduct post order on tree
void post_order(Node *root, Code table[static ALPHABET], Code *c) {
    if (root) {
        uint8_t x; //store popped bits

        if (!(root->left) && !(root->right)) { //check for children
            table[root->symbol] = *c; //add code to table
        } else {
            code_push_bit(c, (uint8_t) 0); //push 0 to code
            post_order(root->left, table, c); //traverse left node
            code_pop_bit(c, &x);

            code_push_bit(c, (uint8_t) 1); //push 1 to code
            post_order(root->right, table, c); //traverse right node
            code_pop_bit(c, &x);
        }
    }
}
void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init(); //create new code
    post_order(root, table, &c);
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
    Stack *s = stack_create((uint32_t) nbytes);
    for (uint32_t i = 0; i < nbytes; i++) {
        if (tree_dump[i] == ((uint8_t) 'L')) { //check for leaf
            Node *n = node_create(tree_dump[i + 1], 0); //create node of out leaf sym
            stack_push(s, n); //add to stack
            i++;
        } else if (tree_dump[i] == ((uint8_t) 'I')) {
            Node *l; //left node
            Node *r; //right node
            stack_pop(s, &r); //pop right node
            stack_pop(s, &l); //pop left node
            Node *p = node_join(l, r); //join to form parent node
            stack_push(s, p); //push parent node
        }
    }
    Node *t;
    stack_pop(s, &t);
    stack_delete(&s);
    return t;
}

void delete_tree(Node **root) {
    if (*root) {
        if ((*root)->left && (*root)->right) { //delete any children
            delete_tree(&(*root)->left);
            delete_tree(&(*root)->right);
            node_delete(root);
        } else {
            node_delete(root);
        }
    }
}
