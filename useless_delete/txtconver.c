//this file was made by sushant.

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *binFile, *txtFile;
    char ch;

    // Open the binary file in read binary mode
    binFile = fopen("output.bin", "rb");
    if (binFile == NULL) {
        perror("Error opening output.bin");
        return 1;
    }

    // Open the text file in write mode
    txtFile = fopen("recovered.txt", "w");
    if (txtFile == NULL) {
        perror("Error creating recovered.txt");
        fclose(binFile);
        return 1;
    }

    // Read from binary file and write to text file
    while (fread(&ch, sizeof(char), 1, binFile) == 1) {
        fputc(ch, txtFile);
    }

    printf("Conversion complete: output.bin -> recovered.txt\n");

    // Close the files
    fclose(binFile);
    fclose(txtFile);

    return 0;
}
