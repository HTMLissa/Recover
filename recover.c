#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

typedef uint8_t BYTE;

// Define number of bytes in each block
#define BLOCK_SIZE 512
// Declare bool to check whether JPEG has been found yet
bool image_already_found = false;
// Declare variable to keep track of number of JPEG's found in the file
int jpgs_found = 0;
// Declare variable to keep track of the file names
char filename[8];
// Declare current file
FILE *current_file = NULL;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover.c image\n");
        return 1;
    }

    // Check whether forensic file can be opened properly
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Declare buffer
    BYTE buffer[BLOCK_SIZE];

    // Read memory card
    while (fread(buffer, BLOCK_SIZE, 1, card) == true)
    {
        // If start of new JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If first JPEG has not been found yet
            if (jpgs_found == 0)
            {
                image_already_found = true;
            }

            // If first JPEG has already been found
            if (jpgs_found > 0)
            {
                // Close current file
                fclose(current_file);
            }
            // Create new JPEG
            sprintf(filename, "%03i.jpg", jpgs_found);
            current_file = fopen(filename, "w");
            fwrite(&buffer, BLOCK_SIZE, 1, current_file);
            jpgs_found++;
        }
        else if (image_already_found)
        {
            fwrite(&buffer, BLOCK_SIZE, 1, current_file);
        }
    }
    // Close files
    fclose(current_file);
    fclose(card);
    return 0;
}