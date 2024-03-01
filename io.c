//Hersh Rudrawal
//CSE13s
#include "io.h"

#include "code.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//This file contain fucntion that can read bytes off a file, write bytes to a file, write codes to a file.

uint64_t bytes_read = 0; //total number of bytes read
uint64_t bytes_written = 0; //total number of bytes written

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int64_t bytes = 0; //store total bytes
    while (bytes < nbytes) {
        int64_t x = 0; //store bytes read from file
        if ((x = read(infile, buf, nbytes - bytes)) < 1) { //read returns the number of bytes read
            break;
        }
        bytes_read += x;
        bytes += x; //else add x to bytes
    }
    return bytes; //return number of bits
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes = 0;
    while (bytes < nbytes) {
        int32_t x = 0;
        if ((x = write(outfile, buf, nbytes - bytes))
            < 1) { //write returns the number of bytes written
            break;
        }
        bytes_written += x;
        bytes += x; // add x to bytes
    }
    return bytes; //return number of bits
}
bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buffer[BLOCK];
    static uint32_t index = 0; //no of bits
    if (index == 0) {
        if ((read_bytes(infile, buffer, BLOCK)) == 0) { //refill buffer
            return false; //false if no more bytes to read
        }
    }
    uint32_t l = index / 8; //get buffer index
    uint32_t p = index % 8; //get bit position
    uint8_t b = 1 << p; //move b into positon
    b &= (buffer[l]); //AND operation to get bit
    *bit = b >> p; //set bit
    index = (index + 1) % (BLOCK * 8); //increment index, with wrap around
    return true;
}
static uint8_t BUFFER[BLOCK] = { 0 }; //holds code bits
static uint32_t INDEX = 0; //track num bits
void write_code(int outfile, Code *c) {
    if (c) {
        for (uint32_t i = 0; i < c->top; i++) {
            if (INDEX == BLOCK * 8) {
                write_bytes(outfile, BUFFER, BLOCK); //write out block
                INDEX = 0; //reset index
            }
            uint32_t l = i / 8; //get byte
            uint32_t p = i % 8; //get position
            uint8_t b = 1 << p; //move b to positon
            b &= c->bits[l]; //get bit from code
            b = b >> p; //move b back
            uint32_t L = INDEX / 8; //byte in buffer
            uint32_t P = INDEX % 8; //positon in buffer
            if (b == 0) { //clear bit to 0
                b = 1 << P;
                BUFFER[L] &= ~b; //AND operation on desired bit with 0, rest remain unchanged
            } else { //set bit to 1
                b = 1 << P;
                BUFFER[L] |= b;
            }
            INDEX++;
        }
    }
}
//write the remain codes to the file
void flush_codes(int outfile) {
    if (INDEX > 0) {
        for (uint32_t i = INDEX; i < BLOCK * 8; i++) { //clear bits passed the index
            uint32_t l = i / 8; //get byte
            uint32_t p = i % 8; //get position
            uint8_t b = 1 << p; //move b to positon
            BUFFER[l] &= ~b;
        }
        write_bytes(outfile, BUFFER, ((INDEX - 1) / 8) + 1); //write bytes up till the index;
        INDEX = 0;
    }
}
