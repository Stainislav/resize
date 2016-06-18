/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 *
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // convert resize degree to an int
    int resize_number = atoi(argv[1]);
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
     
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // make new headers
    BITMAPFILEHEADER new_bf = bf;
    BITMAPINFOHEADER new_bi = bi;
    
    // change the height and the width for an outfile
    new_bi.biHeight = abs(bi.biHeight) * resize_number;
    new_bi.biWidth = bi.biWidth * resize_number;
    
    // padding for infile
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
        
    // padding for an outfile
    int new_padding =  (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // change biSizeImage and bfSize
    new_bi.biSizeImage = ((bi.biWidth * resize_number * sizeof(RGBTRIPLE)) + new_padding) * new_bi.biHeight;
    new_bf.bfSize = new_bi.biSizeImage + 54;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // iterate over infile's scanlines
    for (int i = 0; i < abs(bi.biHeight); i++)
    {
        for (int a = 0; a < resize_number; a++)
        {   
            
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                for (int k = 0; k < resize_number; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }                   
            }
            
            // add the new padding
            for (int b = 0; b < new_padding; b++)
            {
                fputc(0x00, outptr);
            }
            
            // go over remaing padding
            fseek(inptr, (54 + ((bi.biWidth * sizeof(RGBTRIPLE) + padding) * i)), SEEK_SET);
        }
    }
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
