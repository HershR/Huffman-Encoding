//Hersh Rudrawal
//CSE13s
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

//This file containes the implemntation of the huffman decoder.
//It takes in a file, checks if it contains the appropriate header
//If it does then program will read the tree dump out of the file and reconstruct the tree
//with the newly constructed tree, the program writes out the decoded files to a desired file

uint64_t bytes_written; //total bytes written
uint64_t bytes_read; //total bytes read

int main(int argc, char **argv) {
    int infile = STDIN_FILENO; //default input
    int outfile = STDOUT_FILENO; //default output
    int opt = 0; //store options
    bool stats = false; //check to prints statistics
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': //help
            fprintf(stdout, "SYNOPSIS\n A Huffman decoder.\n Decompresses a file using the Huffman "
                            "coding algorithm.\n");
            fprintf(stdout, "USAGE\n ./decode [-h] [-i infile] [-o outfile]\n");
            fprintf(stdout,
                "OPTIONS\n -h \t\tProgram usage and help\n -v \t\tPrint compression stats\n -i "
                "infile \tInput file to decompress\n -o \t\tOutput of decompressed data.\n");
            return 1;
            break;
        case 'v': //print stats
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
            fprintf(stderr, "SYNOPSIS\n A Huffman decoder.\n Decompresses a file using the Huffman "
                            "coding algorithm.\n");
            fprintf(stderr, "USAGE\n ./decode [-h] [-i infile] [-o outfile]\n");
            fprintf(stderr,
                "OPTIONS\n -h \t\tProgram usage and help\n -v \t\tPrint compression stats\n -i "
                "infile \tInput file to decompress\n -o \tOutput of decompressed data.\n");
            return 1;
        }
    }

    //read header from file and check for MAGIC number
    Header h;
    read_bytes(infile, (uint8_t *) &h, sizeof(Header));
    if (h.magic != MAGIC) { //if the file does not contain the header
        fprintf(stderr, "not a proper input file\n");
        close(outfile);
        close(infile);
        exit(1);
        return 1;
    }

    fchmod(outfile, h.permissions); //set outfile permission
    uint8_t dump[h.tree_size]; //store tree dump

    bytes_written = 0;
    bytes_read = 0;

    read_bytes(infile, dump, h.tree_size); //read tree dump

    Node *root = rebuild_tree(h.tree_size, dump); //rebuild tree

    //decode the file. read a bit from file and traverse tree
    //inspired from eugene's section
    Node *curr = root;
    uint8_t bit = 0;
    while (bytes_written < h.file_size) {
        if (curr) {
            if (!(curr->left) && !(curr->left)) { //check if leaf
                write_bytes(
                    outfile, &(curr)->symbol, sizeof(curr->symbol)); //write symbol to output
                curr = root;
            } else {
                if (read_bit(infile, &bit)) {
                    if (bit == 0) {
                        curr = curr->left; //travers left side if 0
                    } else if (bit == 1) {
                        curr = curr->right; //else right side
                    }
                } else {
                    break;
                }
            }
        }
    }

    //print stats
    if (stats) {
        fprintf(stderr, "Compressed file size: %ld bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file size: %ld bytes\n", bytes_written);
        // space saving: 100 * (1 − (compressed size/uncompressed size))
        double space = 100 * (1 - ((double) bytes_read / (double) bytes_written));
        fprintf(stderr, "space saved: %4.2f%%\n", space);
    }

    //delete tree and close files
    delete_tree(&root);
    close(infile);
    close(outfile);
    exit(1);
    return 0;
}
