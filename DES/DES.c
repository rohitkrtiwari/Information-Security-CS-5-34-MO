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
void toBlock(char *input, uint64_t **blocks, size_t *num_blocks){
	size_t length = strlen(input);

	// Pad the string if it's not a multiple of 8 Bytes
	pad_string(input, &length);

	// Calculate the number of 64-bit blocks
	*num_blocks = length / DES_BLOCK_SIZE;

	*blocks = (uint64_t *)malloc(*num_blocks*sizeof(uint64_t));

	// Convert each 8-byte segment into a 64-bit unsigned integer
	for(size_t i=0;i<*num_blocks;i++){
		(*blocks)[i] = 0;
		for(size_t j=0;j<DES_BLOCK_SIZE; j++) (*blocks)[i] |= (uint64_t)(unsigned char)input[i * DES_BLOCK_SIZE + j] << (8 * (DES_BLOCK_SIZE - 1 - j));
	}


}

// Function to print the 64-bit blocks in HExadecimal Format ( for debugging)
void print_blocks(uint64_t *blocks, size_t num_blocks){
	for(size_t i=0;i<num_blocks; i++){
		printf("Blocks %zu: ", i+1);
		for(int j=63;j>=0;j--){
			printf("%lu", (blocks[i] >> j) & 1);
			if(j%8 == 0) printf(" ");
		}
		printf("\n");
	}
}

void rotate_left(uint64_t *block){
	*block = (*block << 1) | (*block >> 63);
}

int main() {
	char str[] = "the main written part of a book, newspaper, etc. (not the pictures, notes, index, etc.) the written form of a speech, interview, etc.";
	uint64_t *blocks = NULL;
	size_t num_blocks = 0;
	toBlock(str, &blocks, &num_blocks);
	print_blocks(blocks, num_blocks);

	printf("\nAfter Rotating left 1 bit: \n");
	rotate_left(&blocks[0]);

	// Print the modified blocks
	print_blocks(blocks, num_blocks);

	free(blocks);

	return 0;
}
