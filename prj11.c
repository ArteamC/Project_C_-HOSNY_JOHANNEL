#include "prj11.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// === Chargement image BMP 8 bits ===
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Unable to open file %s\n", filename);
        return NULL;
    }

    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (!img) {
        fclose(f);
        return NULL;
    }

    fread(img->header, sizeof(unsigned char), 54, f);
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    if (img->colorDepth != 8) {
        printf("Only 8-bit grayscale BMP files are supported.\n");
        fclose(f);
        free(img);
        return NULL;
    }

    fread(img->colorTable, sizeof(unsigned char), 1024, f);

    img->data = malloc(img->dataSize);
    fread(img->data, sizeof(unsigned char), img->dataSize, f);

    fclose(f);
    return img;
}

// === Sauvegarde ===
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Unable to save to %s\n", filename);
        return;
    }

    fwrite(img->header, sizeof(unsigned char), 54, f);
    fwrite(img->colorTable, sizeof(unsigned char), 1024, f);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, f);

    fclose(f);
}

// === Libération mémoire ===
void bmp8_free(t_bmp8 *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

// === Infos ===
void bmp8_printInfo(const t_bmp8 *img) {
    printf("\nImage information:\n");
    printf("Width        : %u pixels\n", img->width);
    printf("Height       : %u pixels\n", img->height);
    printf("Color Depth  : %u bits\n", img->colorDepth);
    printf("Image Size   : %u bytes\n", img->dataSize);
}

// === Filtres simples ===
void bmp8_negative(t_bmp8 *img) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness(t_bmp8 *img, int value) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int temp = img->data[i] + value;
        img->data[i] = (temp > 255) ? 255 : (temp < 0 ? 0 : (unsigned char)temp);
    }
}

void bmp8_threshold(t_bmp8 *img, int threshold) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}

// === Convolution générique ===
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int n = kernelSize / 2;
    unsigned char *newData = malloc(img->dataSize);
    if (!newData) {
        printf("Memory error during filter.\n");
        return;
    }

    for (unsigned int y = 0; y < img->height; y++) {
        for (unsigned int x = 0; x < img->width; x++) {
            float pixel = 0.0f;
            for (int ky = -n; ky <= n; ky++) {
                for (int kx = -n; kx <= n; kx++) {
                    int ix = x + kx;
                    int iy = y + ky;
                    if (ix >= 0 && ix < (int)img->width && iy >= 0 && iy < (int)img->height) {
                        pixel += img->data[iy * img->width + ix] * kernel[ky + n][kx + n];
                    }
                }
            }
            if (pixel < 0) pixel = 0;
            if (pixel > 255) pixel = 255;
            newData[y * img->width + x] = (unsigned char)roundf(pixel);
        }
    }

    // Remplacer ancienne image
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = newData[i];
    }
    free(newData);
}

// === Filtres prédéfinis ===
void bmp8_boxBlur(t_bmp8 *img) {
    float box[3][3] = {
        {1/9.f, 1/9.f, 1/9.f},
        {1/9.f, 1/9.f, 1/9.f},
        {1/9.f, 1/9.f, 1/9.f}
    };
    float* kernel[3] = { box[0], box[1], box[2] };
    bmp8_applyFilter(img, kernel, 3);
}

void bmp8_gaussianBlur(t_bmp8 *img) {
    float gauss[3][3] = {
        {1/16.f, 2/16.f, 1/16.f},
        {2/16.f, 4/16.f, 2/16.f},
        {1/16.f, 2/16.f, 1/16.f}
    };
    float* kernel[3] = { gauss[0], gauss[1], gauss[2] };
    bmp8_applyFilter(img, kernel, 3);
}

void bmp8_outline(t_bmp8 *img) {
    float outline[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    float* kernel[3] = { outline[0], outline[1], outline[2] };
    bmp8_applyFilter(img, kernel, 3);
}

void bmp8_emboss(t_bmp8 *img) {
    float emboss[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };
    float* kernel[3] = { emboss[0], emboss[1], emboss[2] };
    bmp8_applyFilter(img, kernel, 3);
}

void bmp8_sharpen(t_bmp8 *img) {
    float sharpen[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    float* kernel[3] = { sharpen[0], sharpen[1], sharpen[2] };
    bmp8_applyFilter(img, kernel, 3);
}