#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DES_BLOCK_SIZE 8
#define NUM_ROUNDS 16
#define ROUNDS 16

int encTable [] = {45, 12, 3, 60, 22, 7, 34, 18, 50, 29, 61, 5, 41, 27, 14, 56, 38, 9, 63, 1, 48, 20, 33, 16, 52, 25, 42, 8, 57, 19, 46, 11, 30, 4, 59, 24, 37, 15, 51, 26, 44, 10, 62, 2, 35, 17, 49, 28, 53, 21, 40, 6, 58, 23, 36, 13, 47, 0, 55, 32, 43, 31, 54, 39};
int decTable [] = {57, 19, 43, 2, 33, 11, 51, 5, 27, 17, 41, 31, 1, 55, 14, 37, 23, 45, 7, 29, 21, 49, 4, 53, 35, 25, 39, 13, 47, 9, 32, 61, 59, 22, 6, 44, 54, 36, 16, 63, 50, 12, 26, 60, 40, 0, 30, 56, 20, 46, 8, 38, 24, 48, 62, 58, 15, 28, 52, 34, 3, 10, 42, 18};

int PC1[] = {
	57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};

int PC2[] = {
	14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

int keyShifts[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};


// Expansion Table (E) - Expands 32-bit R to 48-bit
int E_table[48] = {
	31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8,
	7, 8, 9, 10, 11, 12, 11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20, 19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0
};

// P-Box Permutation Table (P)
int P_table[32] = {
	15, 6, 19, 20, 28, 11, 27, 16,
	0, 14, 22, 25, 4, 17, 30, 9,
	1, 7, 23, 13, 31, 26, 2, 8,
	18, 12, 29, 5, 21, 10, 3, 24
};

// S-Boxes (8 S-boxes, each 4x16)
int S_boxes[8][4][16] = {
    { // S1
	{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    { // S2
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    // Remaining S-boxes omitted for brevity...
};


// Expand the 32-bit input R to 48-bit output
uint64_t expand(uint32_t R) {
	uint64_t expanded = 0;
	for(int i=0;i<48;i++) expanded |= ((R >> E_table[i]) & 1) << (47 - i);
	return expanded;
}

// S-Box Substitution: Converts 48-bit input to 32-bit output
uint32_t s_box_substitution(uint64_t expanded){
	uint32_t result = 0;
	for(int i=0;i<8;i++){
		int row = ((expanded >> (42 - i*6)) & 0x01) << 1 | ((expanded >> (47 - i*6)) & 0x01);
		int col = (expanded >> (43 - i*6)) & 0x0F;
		result |= S_boxes[i][row][col] << (28 - i*4);
	}
	return result;
}


// P-Box Permutation
uint32_t permute(uint32_t input){
	uint32_t output = 0;
	for(int i=0;i<32;i++){
		output |= ((input >> P_table[i]) & 1) << (31 - i);
	}
	return output;
}

// Feistal Function (F-function) Implementation
uint32_t feistal_function(uint32_t R, uint64_t subkey) {
	uint64_t expanded = expand(R);
	expanded ^= subkey;	// XOR with 48-bit subkey
	uint32_t s_box_output = s_box_substitution(expanded);
	return permute(s_box_output);
}

// Left Circular Shift
uint32_t left_circular_shift(uint32_t half, int shifts) {
	return ((half << shifts) | (half >> (28 - shifts))) & 0x0FFFFFFF;
}

// Function to generate 16 subkeys
void generate_subkeys(uint64_t key, uint64_t subkeys[NUM_ROUNDS]) {
	uint64_t permuted_key = 0;

	// Apply PC-1 to generate 16 subkeys 56-bit key
	for(int i=0;i<56; i++){
		permuted_key |= ((key >> (64 - PC1[i])) & 1) << (55 - i);
	}

	// Split into two 28-bit halves
	uint32_t C = (permuted_key >> 28) & 0x0FFFFFFF;
	uint32_t D = permuted_key & 0x0FFFFFFF;

	// Generate 16 subkeys
	for(int round = 0; round < NUM_ROUNDS; round++) {
		// Perform left circular shift
		C = left_circular_shift(C, keyShifts[round]);
		D = left_circular_shift(D, keyShifts[round]);

		// Combine C and D (56-bit)
		uint64_t combined = ((uint64_t)C << 28) | D;

		// Apply PC-2 to get 48-bit Subkey
		uint64_t subkey = 0;
		for(int j=0;j<48;j++) {
			subkey |= ((combined >> (56 - PC2[j])) & 1) << (47 - j);
		}

		subkeys[round] = subkey;
	}
}

// Function to print the subkeys (Debugging)
void print_subkeys(uint64_t subkeys[NUM_ROUNDS]) {
	for(int i=0;i<NUM_ROUNDS; i++){
		printf("Subkey %2d: %012lx\n", i+1, subkeys[i]);
	}
}

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

void initial_permutation(uint64_t *blocks, size_t num_blocks){
	for(size_t i=0;i<num_blocks; i++){
		uint64_t permuted_value = 0;

		for(int j=0;j<DES_BLOCK_SIZE*8;j++){
			int bit_position = encTable[j];
			uint64_t bit = (blocks[i] >> bit_position) & 1;
			permuted_value |= (bit << j);
		}
		blocks[i] = permuted_value;
	}
}

void reverse_permutation(uint64_t *blocks, size_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        uint64_t reversed_value = 0;  // Reset for each block

        for (int j = 0; j < 64; j++) {  // 64-bit block
            int bit_position = decTable[j];  // No need to subtract 1 (your table is 0-based)
            uint64_t bit = (blocks[i] >> bit_position) & 1;  // Extract bit
            reversed_value |= (bit << j);  // Place bit in correct position
        }

        blocks[i] = reversed_value;  // Store reversed (original) value
    }
}

// Left rotation schedule for key halves
uint32_t rotate_left_28(uint32_t val, int shifts) {
	return ((val << shifts) | (val >> (28 - shifts))) & 0x0FFFFFFF;
}

// Function to perform DES encryption
uint64_t des_encrypt(uint64_t *plaintext, uint64_t subkeys[16]) {
	uint64_t ip_text = 0;
	for(int i=0;i<64;i++){
		ip_text |= ((*plaintext >> (63 - encTable[i])) & 1) << (63 - i);
	}

	uint32_t L = (ip_text >> 32) & 0x0FFFFFFF;
	uint32_t R = ip_text & 0x0FFFFFFF;

	for(int i=0;i<ROUNDS; i++){
		uint32_t new_R = L ^ feistal_function(R, subkeys[i]);
		L = R;
		R = new_R;
	}

	uint64_t pre_output = ((uint64_t)R << 32) | L;
	uint64_t cipher = 0;
	for(int i=0;i<64;i++) {
		cipher |= ((pre_output >> (63 - decTable[i])) & 1) << (63 - i);
	}
	return cipher;
}

// Function to perform DES decryption (reverse subkey order)
uint64_t des_decrypt(uint64_t ciphertext, uint64_t subkeys[16]) {
	uint64_t ip_text = 0;
	for(int i=0; i<64; i++) {
		ip_text |= ((ciphertext >> (63 - encTable[i])) & 1) << (63 - i);
	}

	uint32_t L = (ip_text >> 32) & 0x0FFFFFFFF;
	uint32_t R = ip_text & 0x0FFFFFFFF;

	for(int i=ROUNDS-1; i>=0; i--){
		uint32_t new_R = L ^ feistal_function(R, subkeys[i]);
		L = R;
		R = new_R;
	}

	uint64_t pre_output = ((uint64_t)R << 32) | L;
	uint64_t plaintext = 0;
	for(int i=0; i<64; i++) {
		plaintext |= ((pre_output >> (63 - decTable[i])) & 1) << (63 - i);
	}
	return plaintext;
}

void des_encrypt_all(uint64_t *blocks, size_t num_blocks, uint64_t subkeys[NUM_ROUNDS]) {
    for (size_t i = 0; i < num_blocks; i++) {
        blocks[i] = des_encrypt(&blocks[i], subkeys);
    }
}

void des_decrypt_all(uint64_t *blocks, size_t num_blocks, uint64_t subkeys[NUM_ROUNDS]) {
	for(size_t i=0;i<num_blocks; i++){
		blocks[i] = des_decrypt(blocks[i], subkeys);
	}
}

void blocks_to_strings(uint64_t *blocks, size_t num_blocks, char *output){
	for(size_t i=0;i<num_blocks; i++){
		for(int j=0; j<DES_BLOCK_SIZE; j++) {
			output[i * DES_BLOCK_SIZE + j] = (char)((blocks[i] >> (8 * (DES_BLOCK_SIZE - 1 - j))) & 0x0FF);
		}
	}
	output[num_blocks * DES_BLOCK_SIZE] = '\0';
}

void remove_padding(char *output, size_t *len) {
	size_t padding_length = output[*len -1];
	if(padding_length > 0 && padding_length <= DES_BLOCK_SIZE) {
		output[*len - padding_length] = '\0';
		*len -= padding_length;
	}
}

void reverse_subkeys(uint64_t subkeys[NUM_ROUNDS]) {
	for (int i=0;i<NUM_ROUNDS/2; i++){
		uint64_t temp = subkeys[i];
		subkeys[i] = subkeys[NUM_ROUNDS - 1 - i];
		subkeys[NUM_ROUNDS - 1 - i] = temp;
	}
}

int main() {
    char str[] = "the main written part of a book, newspaper, etc. (not the pictures, notes, index, etc.) the written form of a speech, interview, etc.";

    // Key Initialization
    uint64_t key = 0x133457799BBCDFF1;
    uint64_t subkeys[NUM_ROUNDS];
    generate_subkeys(key, subkeys);
    print_subkeys(subkeys);

    // Plain Text to 64-bit Blocks
    uint64_t *blocks = NULL;
    size_t num_blocks = 0;
    toBlock(str, &blocks, &num_blocks);

    // Print Original Blocks
    printf("Original Blocks:\n");
    print_blocks(blocks, num_blocks);


    // Encryption
    initial_permutation(blocks, num_blocks);
    des_encrypt_all(blocks, num_blocks, subkeys);
    reverse_permutation(blocks, num_blocks);

    // Print Encrypted Blocks
    printf("Encrypted Blocks:\n");
    print_blocks(blocks, num_blocks);

    // Decryption
    reverse_subkeys(subkeys);  // Reverse subkeys for decryption
    initial_permutation(blocks, num_blocks);
    des_decrypt_all(blocks, num_blocks, subkeys);
    reverse_permutation(blocks, num_blocks);

    // Print Decrypted Blocks
    printf("Decrypted Blocks:\n");
    print_blocks(blocks, num_blocks);


    // Convert Decrypted Blocks to String
    char *decrypted_message = (char *)malloc(num_blocks * DES_BLOCK_SIZE + 1);
    blocks_to_strings(blocks, num_blocks, decrypted_message);

    // Print Decrypted Message Before Removing Padding
    printf("Decrypted Message (Before Removing Padding): %s\n", decrypted_message);

    // Remove Padding
    size_t decrypted_length = num_blocks * DES_BLOCK_SIZE;
    remove_padding(decrypted_message, &decrypted_length);

    // Print Decrypted Message
    printf("Decrypted Message: %s\n", decrypted_message);

    // Free Memory
    free(blocks);
    free(decrypted_message);

    return 0;
}
