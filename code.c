//Hersh Rudrawal
//CSE13s Spring 2021
#include "code.h"

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//This file contains the implentation of the code ADT

//initalizes code
Code code_init(void) {
    Code c;
    c.top = 0; //number of bits in code
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = (uint8_t) 0;
    }
    return c;
}
uint32_t code_size(Code *c) { //get size of a code
    return c->top;
}
bool code_empty(Code *c) { //check if a code is empty
    return c->top == 0;
}
bool code_full(Code *c) { //check if code is full
    return c->top == ALPHABET;
}
//based off bitvector code from Asignment 5
//inspired by eugene's section

//add bit to code
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    uint32_t l = c->top / 8; //get current byte we are writing to
    uint32_t p = c->top % 8; //get open bit postion

    if (bit == 0) { //clear bit
        bit = 1 << p; //shift bit by p
        c->bits[l] &= ~bit; // AND operation to clr bit
    } else { //set bit
        bit = 1 << p; //shift bit by p
        c->bits[l] |= bit; // OR operation to set bit to 1
    }
    c->top++; //increment top
    return true;
}
//pop bit from code
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top--; //decremnt top
    uint32_t l = c->top / 8; //get current byte
    uint32_t p = c->top % 8; //get bit postion
    uint8_t b = 1 << p; //shift 1 by p
    b &= (c->bits[l]); // AND operation to get bit
    *bit = b >> p; //shift b back into position

    return true;
}
void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        uint32_t l = i / 8;
        uint32_t p = i % 8;
        uint8_t b = 1 << p;
        b &= (c->bits[l]);
        b = b >> p;
        printf("%d ", b);
        if (i + 1 % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
