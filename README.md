# Huffman Encoding
Programming assignment from 2021

encode.c
This program reads a file and compresses it using huffman encoding. The encoded data is then written to a desired location. 
The program takes the following options: -h help,-i infile(file to read from- default stdin) -o outfile(file to print to- default stdout), -v print stats(uncompressed file size, compressed file size, and space saving).

decode.c
This program reads a file encoded by the encoder and reconstucts the original file to a desired location.
The program takes the following options: -h help,-i infile(file to read from- defualt stdin) -o outfile(file to print to- default stdout), -v print out stats(uncompressed file size, compressed file size, and space saving).

## Build
	$ make all
## Running
	$ ./encode [-v,-h] [-i file name] [-o file name]
	$ ./decode [-v,-h] [-i file name] [-o file name]
## Cleaning
	$ make clean
## Other files
	node.c		- contains the implementation of the node
	code.c		- contains the implementation of the code
	pq.c		- contains the implementation of the priority queue ADT
	stack.c		- contains the implementation of the stack ADT
	io.c		- used by encode and decode to read bytes and bits, and write bytes and codes to and from a file
	huffman.c	- used by encode and decode to build trees, reconstuct trees, delete trees, and build codes
	entropy.c	- calculates the entropy of a file
	DESIGN.PDF	- contains psudeocode for node.c, code.c, pq.c, stack.c, io.c, huffman.c, encode.c, decode.c
	
## Problems
	My decoder is significantly slower than the resource decoder for larger files.
	Valgrind does not detect any memory leaks on encoder but for large files it gives a -  client switching stacks warning.
