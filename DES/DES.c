#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DES_BLOCK_SIZE 8

// Function to pad the input string to ensure it's a multiple of 8 bytes ( 64 bits )
void pad_string(char *input, size_t *len){
	size_t input_length = strlen(input);
	size_t padding_needed = DES_BLOCK_SIZE - (input_length % DES_BLOCK_SIZE);

	// If padding is needed, add extra bytes
	if(padding_needed != DES_BLOCK_SIZE){
		memset(input + input_length, padding_needed, padding_needed);
		*len = input_length + padding_needed;
	}
}


// Function to convert string data to 64-bit blocks (Desgined for DES Processing)
void toBlock(char *input, unsigned char **blocks, size_t *num_blocks){
	size_t length = strlen(input);

	// Pad the string if it's not a multiple of 8 Bytes
	pad_string(input, &length);

	// Calculate the number of 64-bit blocks
	*num_blocks = length / DES_BLOCK_SIZE;

	*blocks = (unsigned char *)malloc(length);

	for(size_t i=0;i<length; i++) (*blocks)[i] = input[i];
}

// Function to print the 64-bit blocks in HExadecimal Format ( for debugging)
void print_blocks(unsigned char *blocks, size_t num_blocks){
	for(size_t i=0;i<num_blocks; i++){
		printf("Blocks %zu: ", i+1);
		for(int j=0;j<DES_BLOCK_SIZE;j++) printf("%02X", blocks[i*DES_BLOCK_SIZE +j]);
		printf("\n");
	}
}

int main() {
    char str[] = "the main written part of a book, newspaper, etc. (not the pictures, notes, index, etc.) the written form of a speech, interview, etc.";
    size_t len = strlen(str);
    const char *key = "12345678";

	unsigned char *blocks = NULL;
	size_t num_blocks = 0;
	toBlock(str, &blocks, &num_blocks);
	print_blocks(blocks, num_blocks);
}
