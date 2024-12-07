#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

const int LINE_LENGTH = 10, PALETTE_SIZE = 8;

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

void writeTwoBitsRow(char* row, FILE* file){
    char mask = 1; //first bit at 1, others at 0
    char output;
    for(int j = 0; j<2; j++){
        output = 0;
        for(int i = 0; i<8; i++){
            output = output | ((row[i] & mask)<< 7 -i -j); //start at the end of the byte and shift cursor to 1 bit each time
            printf("%d ",i);
            printBits(sizeof(output), &output);
        }
        fwrite(&output,sizeof(output),1,file);
        mask<<=1;
    }
}

int main(){
    FILE *spriteFile, *binFile;
    // Open a file in read mode
    spriteFile = fopen("test.ppm", "r");

    // Open a file in write mode
    binFile = fopen("output.bin", "wb"); 

    if(spriteFile == NULL){
        printf("Impossible d'ouvrir ce fichier\n");
        exit(0);
    }

    char line[LINE_LENGTH];
    
    while(fgets(line, LINE_LENGTH, spriteFile)&& strcmp("255\n",line) != 0);

    int i = 0;
    int pixelIndex = 0;
    int rowIndex = 0;
    
    char cur = 0;
    char colIndex;
    uint8_t colCode[3];
    uint16_t colorPalette[PALETTE_SIZE];
    char tile[8][8];

    for(int j = 0; j<PALETTE_SIZE; j++){
        colorPalette[j] = 0x8000;
    }

    while(fgets(line, LINE_LENGTH, spriteFile)||i>2||pixelIndex==8){
        if(i>2){
            uint16_t squashedCol = colCode[0]<<7|colCode[1]<<2|colCode[2]>>3;
            i = 0; 
            colIndex = getColorIndex(squashedCol, colorPalette);
            if(pixelIndex == 8){
                printf("compute\n");
                char tab[8] ={0,0,6,0,7,6,7,5};
                writeColRow(tab, binFile);
                pixelIndex = 0;
            }

            if(colIndex == -1){
                //write cur
                colorPalette[cur] = squashedCol;
                colorRow[][pixelIndex] = cur;
                cur++;
            }else{
                //write colIndex;
                colorRow[][pixelIndex] = colIndex;
            }
            pixelIndex++;
        }
        colCode[i] = (atoi(line)>>3)<<3;
        i++;
    }

    fclose(spriteFile);
    fclose(binFile);

    return 0;
}