#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

const int LINE_LENGTH = 10, PALETTE_SIZE = 16;

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

char getColorIndex(uint16_t color, uint16_t* palette){
    for(char i=0; i<PALETTE_SIZE; i++){
        if(palette[i] == color){
            return i;
        }
    }
    return -1;
}

void writeTwoBits(char *row, FILE *file, char mask){
    char output;
    for(int j = 0; j<2; j++){
        output = 0;
        for(int i = 0; i<8; i++){
            output = output | ((row[i] & mask)<< 7 -i -j); //start at the end of the byte and shift cursor to 1 bit each time
        }
        // printBits(sizeof(output), &output);
        fwrite(&output,sizeof(output),1,file);
        mask<<=1;
    }
}

void writePalette(uint16_t *palette, FILE *file){
    //write the palette to the specified file
    fwrite(palette,sizeof(uint16_t), sizeof(palette),file);
}

void concat(char* s1, char* s2, char* ns) {
    ns[0] = '\0';
    strcpy(ns, s1);
    strcat(ns, s2);
}


int main(){
    FILE *spriteFile, *binFile, *paletteFile;

    char inputName[20], filename[25], outputName[25], paletteName[25];
    
    //input from user
    printf("nom du fichier (max 19 caracteres): ");
    if (scanf("%19s", inputName) != 1) { // Ensure input is within bounds
        fprintf(stderr, "Erreur, saisie invalide\n");
        return 1;
    }
    
    concat(inputName,".ppm", filename);     // Name of the file to read
    concat(inputName,".bin", outputName);   // Name of the output file
    concat(inputName,".pal", paletteName);  // Name of the palette file
    
    // Open file in read mode
    spriteFile = fopen(filename, "r");

    // Open a file in write mode
    binFile = fopen(outputName, "wb");

    if(spriteFile == NULL){
        printf("Impossible d'ouvrir ce fichier\n");
        exit(-1);
    }

    char line[LINE_LENGTH];

    while(fgets(line, LINE_LENGTH, spriteFile)&& strcmp("255\n",line) != 0);

    int i = 0;
    int rdVal; //buffer for reading value from file
    int pixelIndex = 0;
    int rowIndex = 0;

    char cur = 0;
    char colorIndex;
    uint8_t colCode[3];
    uint16_t colorPalette[PALETTE_SIZE];
    char tile[8][8];

    for(int j = 0; j<PALETTE_SIZE; j++){
        colorPalette[j] = 0x8000; // Initialize with zero
    }

    while((rdVal = fgetc(spriteFile)) != EOF){
        colCode[i] = rdVal>>3;
        i++;
        if(i>=3){
            // printf("%d %d %d\n",colCode[2],colCode[1],colCode[0]);
            uint16_t squashedCol = colCode[2]<<10|colCode[1]<<5|colCode[0];
            i = 0;

            colorIndex = getColorIndex(squashedCol, colorPalette);
            if(colorIndex == -1){
                //write cur
                if (cur < PALETTE_SIZE){
                    colorPalette[cur] = squashedCol;
                    tile[rowIndex][pixelIndex] = cur;
                    cur++;
                }else{
                    printf("erreur : plus de %d couleurs\n", PALETTE_SIZE);
                    exit(-1);
                }
            }else{
                //write colIndex;
                tile[rowIndex][pixelIndex] = colorIndex;
            }

            pixelIndex++;

            if(pixelIndex > 7){
                writeTwoBits(tile[rowIndex], binFile,1); //mask at 1 (0001)
                pixelIndex = 0;
                // printf("%d\n",rowIndex);
                if(rowIndex > 7){
                    for(int i = 0; i<8; i++){
                        writeTwoBits(tile[i], binFile, 4); //mask at 4 (0100)
                    }
                    rowIndex = 0;
                }else{
                    rowIndex++;
                }
            }
        }
    }if (rdVal == EOF) {
        printf("End of file reached or error occurred.\n");
    }

    fclose(spriteFile);
    fclose(binFile);
    
    paletteFile = fopen(paletteName,"wb");
    writePalette(colorPalette, paletteFile);

    fclose(paletteFile);

    return 0;
}
