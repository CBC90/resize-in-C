// Enlargens a BMP File by a factor of user input

#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize factor infile outfile\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // ensure factor input is valid
    if (n < 0 || n > 100)
    {
        printf("invalid factor input\n");
        return 2;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
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
        return 5;
    }

    // determine padding for input scanlines
    int inpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // save input file Width and Height
    int inbiWidth = bi.biWidth;
    int inbiHeight = bi.biHeight;

    // calculate new Width and Height
    bi.biWidth = bi.biWidth * n;
    bi.biHeight = bi.biHeight * n;

    // save output file Width and Height
    int outbiWidth = bi.biWidth;
    int outbiHeight = bi.biHeight;

    // determine padding for output scanlines
    int outpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // calculate new biSizeImage and bfSize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + outpadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // computation for increasing the image

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(inbiHeight); i < biHeight; i++)
    {

        int k = 0;
        while (k < n)
        {

            // iterate over pixels in scanline
            for (int j = 0; j < inbiWidth; j++)
            {
                // temporary storage
                RGBTRIPLE *triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                for (int m = 0; m < n; m++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // include output padding
            for (int l = 0; l < outpadding; l++)
            {
                fputc(0x00, outptr);
            }

            // reset input cursor to beginning of scanline
            fseek(inptr, -(sizeof(RGBTRIPLE) * inbiWidth), SEEK_CUR);

            k++;
        }
        // moves input cursor to next scanline
        fseek(inptr, ((sizeof(RGBTRIPLE) * inbiWidth) + inpadding), SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
