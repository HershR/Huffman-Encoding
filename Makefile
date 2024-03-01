CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS= -lm

all: encode decode entropy

encode: encode.o huffman.o io.o code.o pq.o node.o stack.o
	$(CC) -o encode $^ $(LFLAGS)

decode: decode.o huffman.o io.o code.o pq.o node.o stack.o
	$(CC) -o decode $^ $(LFLAGS)

entropy: entropy.o
	$(CC) -o entropy $^ $(LFLAGS)
	
clean:
	rm -f encode decode entropy *.o

format:
	clang-format -i -style=file *.h *.c
	
scan-build: clean
	scan-build make
