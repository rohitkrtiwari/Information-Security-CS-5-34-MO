#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE * file;
	unsigned char *buffer;
	long file_size;

	file = fopen("r7.jpg", "rb");
	if(file == NULL) {
		perror("Error opening file");
		return 1;
	}

	// Move the filepointer to the end to get file size
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);

	// Allocate memory to store the file content
	buffer = (unsigned char *) malloc(file_size);
	if(buffer == NULL){
		perror("Memory allocation failed");
		return 1;
	}

	fread(buffer, 1, file_size, file);
	fclose(file);

	// Find the Start of Scan (SOS) marker (FF DA)
	long sos_offset = -1;
	for(long i = 0; i < file_size-1; i++){
		if (buffer[i] == 0xFF && buffer[i+1] == 0xDA){
			sos_offset = i + 2;
			break;
		}
	}

	if(sos_offset != -1) {
		for(long i = sos_offset; i < sos_offset+20; i++){
			printf("%02X ", buffer[i]);
			if ((i+1)%16 == 0) printf("\n");
		}
	}else{
		printf("SOS Marker not found");
	}


	printf("Read %ld bytes  from image file.\n", file_size);

	free(buffer);

	return 0;
}
