//this file was made by sushant.

#include <stdio.h>
#include <stdlib.h>

// Function to convert JPEG to binary
int jpeg_to_bin(const char *inputFilename, const char *outputFilename) {
    FILE *inputFile = NULL, *outputFile = NULL;
    unsigned char *buffer = NULL;
    long fileSize = 0;

    // Open input file
    inputFile = fopen(inputFilename, "rb");
    if (!inputFile) {
        perror("Error opening input JPEG file");
        return 1;
    }

    // Get file size
    if (fseek(inputFile, 0, SEEK_END) != 0 || (fileSize = ftell(inputFile)) < 0) {
        perror("Error determining JPEG file size");
        fclose(inputFile);
        return 1;
    }
    rewind(inputFile);

    // Allocate buffer
    buffer = (unsigned char *)malloc(fileSize);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(inputFile);
        return 1;
    }

    // Read JPEG data
    if (fread(buffer, 1, fileSize, inputFile) != fileSize) {
        perror("Error reading JPEG file");
        free(buffer);
        fclose(inputFile);
        return 1;
    }
    fclose(inputFile);

    // Write to binary file
    outputFile = fopen(outputFilename, "wb");
    if (!outputFile) {
        perror("Error opening output BIN file");
        free(buffer);
        return 1;
    }

    if (fwrite(buffer, 1, fileSize, outputFile) != fileSize) {
        perror("Error writing binary file");
        free(buffer);
        fclose(outputFile);
        return 1;
    }

    printf("Successfully converted '%s' to '%s'\n", inputFilename, outputFilename);

    free(buffer);
    fclose(outputFile);
    return 0;
}

// Function to convert binary back to JPEG
int bin_to_jpeg(const char *inputFilename, const char *outputFilename) {
    FILE *inputFile = NULL, *outputFile = NULL;
    unsigned char *buffer = NULL;
    long fileSize = 0;

    // Open binary input file
    inputFile = fopen(inputFilename, "rb");
    if (!inputFile) {
        perror("Error opening input BIN file");
        return 1;
    }

    // Get file size
    if (fseek(inputFile, 0, SEEK_END) != 0 || (fileSize = ftell(inputFile)) < 0) {
        perror("Error determining BIN file size");
        fclose(inputFile);
        return 1;
    }
    rewind(inputFile);

    // Allocate buffer
    buffer = (unsigned char *)malloc(fileSize);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(inputFile);
        return 1;
    }

    // Read binary data
    if (fread(buffer, 1, fileSize, inputFile) != fileSize) {
        perror("Error reading BIN file");
        free(buffer);
        fclose(inputFile);
        return 1;
    }
    fclose(inputFile);

    // Write to JPEG output file
    outputFile = fopen(outputFilename, "wb");
    if (!outputFile) {
        perror("Error opening output JPEG file");
        free(buffer);
        return 1;
    }

    if (fwrite(buffer, 1, fileSize, outputFile) != fileSize) {
        perror("Error writing JPEG file");
        free(buffer);
        fclose(outputFile);
        return 1;
    }

    printf("Successfully converted '%s' to '%s'\n", inputFilename, outputFilename);

    free(buffer);
    fclose(outputFile);
    return 0;
}

// Example usage
int main() {
    const char *jpegFile = "img.jpg";
    const char *binFile = "image.bin";
    const char *jpegOutput = "reconstructed.jpg";

    // Convert JPEG to BIN
    if (jpeg_to_bin(jpegFile, binFile) != 0) {
        fprintf(stderr, "JPEG to BIN conversion failed.\n");
        return 1;
    }

    // Convert BIN back to JPEG
    if (bin_to_jpeg(binFile, jpegOutput) != 0) {
        fprintf(stderr, "BIN to JPEG conversion failed.\n");
        return 1;
    }

    return 0;
}
