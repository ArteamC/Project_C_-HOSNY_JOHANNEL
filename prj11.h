#ifndef prj11_h
#define prj11_h

#include <stdint.h>

// === Structure pour l'image BMP 8 bits ===
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    unsigned short colorDepth;
    unsigned int dataSize;
} t_bmp8;

// === Fonctions de base ===
t_bmp8 *bmp8_loadImage(const char *filename);
void bmp8_saveImage(const char *filename, t_bmp8 *img);
void bmp8_free(t_bmp8 *img);
void bmp8_printInfo(const t_bmp8 *img);

// === Filtres simples ===
void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);

// === Convolution ===
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

// === Filtres avancés ===
void bmp8_boxBlur(t_bmp8 *img);
void bmp8_gaussianBlur(t_bmp8 *img);
void bmp8_outline(t_bmp8 *img);
void bmp8_emboss(t_bmp8 *img);
void bmp8_sharpen(t_bmp8 *img);

#endif // prj11_h