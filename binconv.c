//This file was made by sushant.

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *txtFile, *binFile;
    char ch;

    // Open the text file in read mode
    txtFile = fopen("input.txt", "r");
    if (txtFile == NULL) {
        perror("Error opening input.txt");
        return 1;
    }

    // Open the binary file in write binary mode
    binFile = fopen("output.bin", "wb");
    if (binFile == NULL) {
        perror("Error creating output.bin");
        fclose(txtFile);
        return 1;
    }

    // Read from text file and write to binary file
    while ((ch = fgetc(txtFile)) != EOF) {
        fwrite(&ch, sizeof(char), 1, binFile);
    }

    printf("Conversion complete: input.txt -> output.bin\n");

    // Close the files
    fclose(txtFile);
    fclose(binFile);

    return 0;
}
