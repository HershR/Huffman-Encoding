//Hersh Rudrawal
//CSE 13s Spring 2021
#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define OPTIONS "hvi:o:"

//This file compresses a file using huffman encoding
//It will read the infile and create a histogram which stores
//the frequency of each unique ascci symbol
//Then the program will convert the historgram into a tree
//It will then create unique codes for each symbol in the tree
//with the codes, it will encode the infile to the out file

//function to create the tree dump
void po_treedump(Node *root, int outfile) {
    if (root) { //check for root
        uint8_t L = (uint8_t) 'L'; //L
        uint8_t I = (uint8_t) 'I'; //I
        if (!(root->left) && !(root->right)) { //check for leaf
            //write L and symbol to file
            write_bytes(outfile, (uint8_t *) &L, sizeof(uint8_t));
            write_bytes(outfile, &root->symbol, sizeof(root->symbol));

        } else {
            //traverse left and right nodes
            po_treedump(root->left, outfile);
            po_treedump(root->right, outfile);
            //since this in a interior node write I
            write_bytes(outfile, (uint8_t *) &I, sizeof(uint8_t));
        }
    }
}

uint64_t bytes_written;
uint64_t bytes_read;
int main(int argc, char **argv) {
    int infile = STDIN_FILENO; //default input
    int outfile = STDOUT_FILENO; //default output
    int opt = 0; //stores options
    bool stats = false; //check to prints statistics
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) { //getoptions
        switch (opt) {
        case 'h': //help
            fprintf(stderr, "SYNOPSIS\n A Huffman encoder.\n Compresses a file using the Huffman "
                            "coding algorithm.\n");
            fprintf(stderr, "USAGE\n ./encode [-h] [-i infile] [-o outfile]\n");
            fprintf(stderr,
                "OPTIONS\n -h \t\tProgram usage and help\n -v \t\tPrint compression stats\n -i "
                "infile \tInput file to compress\n -o \tOutput of compressed data.\n");
            return 0;
            break;
        case 'v': //enable stats printing
            stats = true;
            break;
        case 'i': //set infile
            infile = open(optarg, O_RDONLY);
            if (infile == -1) {
                fprintf(stderr, "fail to open infile");
                return 1;
            }
            break;
        case 'o': //set outfile
            outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (outfile == -1) {
                fprintf(stderr, "fail to open outfile");
                return 1;
            }
            break;
        default: //error
            fprintf(stderr, "SYNOPSIS\n A Huffman encoder.\n Compresses a file using the Huffman "
                            "coding algorithm.\n");
            fprintf(stderr, "USAGE\n ./encode [-h] [-i infile] [-o outfile]\n");
            fprintf(stderr,
                "OPTIONS\n -h \t\tProgram usage and help\n -v \t\tPrint compression stats\n -i "
                "infile \tInput file to compress\n -o \tOutput of compressed data.\n");
            return 1;
        }
    }

    uint64_t hist[ALPHABET] = { 0 }; //histogram, each index represents an ascci value
    hist[0] += 1; //ensure we have at least 2 nodes
    hist[255] += 1;
    int temp
        = open("temp_in.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600); //create temp file to store input
    uint64_t bytes = 0; //store num of bytes read
    uint8_t buffer[BLOCK] = { 0 }; //store bytes
    while ((bytes = read_bytes(infile, buffer, BLOCK)) > 0) { //read a BLOCK of bytes from the file
        write_bytes(temp, buffer, bytes); //write input file to temp file
        for (uint32_t i = 0; i < bytes; i++) { //iterate buffer and update histogram
            hist[buffer[i]] += 1; //increment symbol
        }
    }
    close(temp);

    Node *t = build_tree(hist); // build tree out of histogram

    Code table[ALPHABET] = { 0 }; //table for codes
    build_codes(t, table); //build codes out of tree
    uint32_t size = 0; //num of unique symbols
    for (uint32_t i = 0; i < ALPHABET; i++) { // update size
        if (!code_empty(&table[i])) {
            size++;
        }
    }

    struct stat statbuf;
    fstat(infile, &statbuf); //get infile info
    Header h; //create header
    h.magic = MAGIC; //set magic
    h.permissions = statbuf.st_mode; //set permissions
    fchmod(outfile, statbuf.st_mode); //set outfile permissions

    h.tree_size = (3 * size) - 1; //set tree size
    h.file_size = bytes_read; //set file size

    bytes_written = 0;

    //write everything to temp output file, then write that file to outfile
    int out = open("temp_out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600); //create temp out file
    write_bytes(out, (uint8_t *) &h, sizeof(Header)); //write header to out
    po_treedump(t, out); //write tree dump

    temp = open("temp_in.txt", O_RDONLY); //open temp to read
    lseek(temp, 0, SEEK_SET); //go to beginning of file

    while ((bytes = read_bytes(temp, buffer, BLOCK)) > 0) { //read from file
        for (uint32_t i = 0; i < bytes; i++) {
            write_code(out, &table[buffer[i]]); //write codes to out
        }
    }
    flush_codes(out); //flush remaining codes
    close(out);

    uint64_t written = bytes_written; //store number of bytes written
    bytes_written = 0; //reset
    out = open("temp_out.txt", O_RDONLY); //open temp out to read
    lseek(out, 0, SEEK_SET);

    //write to outfile
    uint8_t OUT[written]; //store all of outfile
    while ((read_bytes(out, OUT, written)) > 0) { //read a BLOCK of bytes from the file
        write_bytes(outfile, OUT, written); //write input file to temp
    }
    //print stats
    if (stats) {
        fprintf(stderr, "Uncompressed file size: %ld bytes\n", h.file_size);
        fprintf(stderr, "Compressed file size: %ld bytes\n", bytes_written);
        // space saving: 100 * (1 − (compressed size/uncompressed size))
        double space = 100 * (1 - ((double) bytes_written / (double) h.file_size));
        fprintf(stderr, "space saved: %4.2f%%\n", space);
    }

    //delete tree and close files
    delete_tree(&t);
    close(infile);
    close(outfile);
    close(temp);
    close(out);
    unlink("temp_out.txt");
    unlink("temp_in.txt");
    exit(0);
    return 0;
}
