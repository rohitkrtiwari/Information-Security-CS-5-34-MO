#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void encryptVigener(char *plainText, char *Key, char *CypherText, size_t m, size_t n){
	for(size_t i = 0, j = 0; i < m && plainText[i]!='\0'; i++){
                if(plainText[i] >= 'A' && plainText[i] <= 'Z'){
                        CypherText[i] = ((plainText[i] - 'A' + (Key[j%n]-'a')) % 26) + 'A';
                        j++;
                } else if(plainText[i] >= 'a' && plainText[i] <= 'z') {
                        CypherText[i] = ((plainText[i] - 'a' + (Key[j%n]-'a')) % 26) + 'a';
                        j++;
                } else {
                        CypherText[i] = plainText[i];
                }
        }
        printf("\nKey used for Encryption: %s", Key);
        printf("\n\nCypher text: %s\n", CypherText);

}

void decryptVigenere(char *CypherText, char* Key, size_t m, size_t n){
	char *plainText;
	plainText = (char *)malloc((m+1)*sizeof(char));
	for (size_t i = 0, j = 0; i < m && CypherText[i] != '\0'; i++) {
        	if (CypherText[i] >= 'A' && CypherText[i] <= 'Z') {
            		// Decrypt uppercase letters
            		plainText[i] = ((CypherText[i] - 'A' - (Key[j % n] - 'a') + 26) % 26) + 'A';
            		j++;  // Increment key index for each alphabetic character
        	} else if (CypherText[i] >= 'a' && CypherText[i] <= 'z') {
            		// Decrypt lowercase letters
            		plainText[i] = ((CypherText[i] - 'a' - (Key[j % n] - 'a') + 26) % 26) + 'a';
            		j++;  // Increment key index for each alphabetic character
        	} else {
            		// Non-alphabetic characters remain unchanged
            		plainText[i] = CypherText[i];
        	}
    	}
    	plainText[m] = '\0';
	printf("\nDecrpyted Text: %s", plainText);
}

int main(){
	char *plainText, *CypherText;
	size_t m, n;
	printf("Please Enter the size of the string: ");
	scanf("%zu", &m);
	getchar();
	plainText = (char *)malloc((m+1) * sizeof(char));
	CypherText = (char *)malloc((m+1) * sizeof(char));
	char Key[] = "kjbvdwdx";
	n = sizeof(Key);
	if(plainText == NULL) {
		fprintf(stderr, "Memory Allocation Failed.\n");
		return 1;
	}

	// Prompt user to enter string
	printf("Please Enter the data: ");
	fgets(plainText, m+1, stdin);
	plainText[strcspn(plainText, "\n")] = '\0';

	encryptVigener(plainText, Key, CypherText, m, n);
	decryptVigenere(CypherText, Key, m, n);

}
