# snes-sprite-converter
Converts any ppm file to a bin file that can be used in snes games

## manual

### input
At first, make sure your image can be divided into tiles of 8 by 8 pixels. this is a necessity for snes to read and display properly.
Also, to work with this programm, you need to put your tiles in one column, so that it can read it vertically.

**In short :** To run properly, this programm needs a ppm file of 8 by n pixels, where n can be divided by 8.

*Advice : create your sprite first, and then cut it in chunks and order them vertically. then export your fil as ".ppm"*

### How to use it
- move your ppm file to the same location as the executable file
- open the executable and write the name of the file **Without the extension**
- a .bin file and a .pal file should have been created

### Output
The .bin file is your actual sprite in bytes that can be used by the snes, and the .pal is the color palette, also needed by the snes.

### Error
if it doesn't work, check that your file is made of chunks that are 8 by 8 pixel, all put in one column.
Any row or column of pixel below that range will cause unexpected result.
