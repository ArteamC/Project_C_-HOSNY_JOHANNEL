//
// Created by Iwan Hosny on 3/21/25.
//
// bmp8.h
#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure représentant une image BMP 8 bits
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

// Fonctions de gestion des fichiers BMP
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (!img) {
        fclose(file);
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    fread(img->header, sizeof(unsigned char), 54, file);
    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    img->width = *(unsigned int*)&img->header[18];
    img->height = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned int*)&img->header[28];
    img->dataSize = *(unsigned int*)&img->header[34];

    if (img->colorDepth != 8) {
        fprintf(stderr, "Image non prise en charge (doit être en 8 bits)\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = malloc(img->dataSize);
    if (!img->data) {
        free(img);
        fclose(file);
        perror("Erreur d'allocation mémoire pour les données image");
        return NULL;
    }

    fread(img->data, sizeof(unsigned char), img->dataSize, file);
    fclose(file);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier pour l'écriture");
        return;
    }
    fwrite(img->header, sizeof(unsigned char), 54, file);
    fwrite(img->colorTable, sizeof(unsigned char), 1024, file);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, file);
    fclose(file);
}

void bmp8_free(t_bmp8 *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

// Traitement : inversion de couleurs
void bmp8_negative(t_bmp8 *img) {
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        img->data[i] = 255 - img->data[i];
    }
}

// Traitement : ajustement de luminosité
void bmp8_brightness(t_bmp8 *img, int value) {
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        int newVal = img->data[i] + value;
        if (newVal > 255) newVal = 255;
        else if (newVal < 0) newVal = 0;
        img->data[i] = (unsigned char)newVal;
    }
}

// Traitement : seuillage
void bmp8_threshold(t_bmp8 *img, int threshold) {
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}

// Filtrage d'image par noyau
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    unsigned char *buffer = malloc(img->dataSize);
    memcpy(buffer, img->data, img->dataSize);

    for (unsigned int y = offset; y < img->height - offset; ++y) {
        for (unsigned int x = offset; x < img->width - offset; ++x) {
            float sum = 0.0f;
            for (int ky = -offset; ky <= offset; ++ky) {
                for (int kx = -offset; kx <= offset; ++kx) {
                    int pixel = img->data[(y + ky) * img->width + (x + kx)];
                    sum += pixel * kernel[ky + offset][kx + offset];
                }
            }
            int result = (int)sum;
            if (result > 255) result = 255;
            if (result < 0) result = 0;
            buffer[y * img->width + x] = (unsigned char)result;
        }
    }

    memcpy(img->data, buffer, img->dataSize);
    free(buffer);
}

#endif
