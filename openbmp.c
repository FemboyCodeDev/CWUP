#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int r;
    int g;
    int b;
    int x;
    int y;
} openPixel;

// New struct to hold the image data and dimensions together :3
typedef struct {
    int width;
    int height;
    int pixelCount;
    openPixel *pixels;
} openImage;

openImage* loadBMP(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // 1. Load file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, file_size, file);
    fclose(file);

    // 2. Parse Headers
    int pixel_data_offset = *(int *)&buffer[10];
    int width             = *(int *)&buffer[18];
    int height            = *(int *)&buffer[22];
    short bpp             = *(short *)&buffer[28];

    int absHeight = (height < 0) ? -height : height;
    int totalPixels = width * absHeight;

    // Allocate the image container
    openImage *img = (openImage *)malloc(sizeof(openImage));
    if (img == NULL) {
        free(buffer);
        return NULL;
    }

    img->width = width;
    img->height = absHeight;
    img->pixelCount = totalPixels;
    img->pixels = (openPixel *)malloc(totalPixels * sizeof(openPixel));

    if (img->pixels == NULL) {
        free(img);
        free(buffer);
        return NULL;
    }

    unsigned char *pixelData = &buffer[pixel_data_offset];

    // 3. Process Pixels based on bit depth
    int index = 0;
    int rowSize24 = ((width * 3 + 3) & ~3);
    int rowSize8  = ((width + 3) & ~3);

    for (int y = 0; y < absHeight; y++) {
        for (int x = 0; x < width; x++) {
            // Determine vertical position (BMPs are usually bottom-up)
            int py = (height > 0) ? (absHeight - 1 - y) : y;
            
            if (bpp == 24) {
                int pos = (py * rowSize24) + (x * 3);
                img->pixels[index].b = pixelData[pos];
                img->pixels[index].g = pixelData[pos + 1];
                img->pixels[index].r = pixelData[pos + 2];
            } 
            else if (bpp == 32) {
                int pos = (py * width * 4) + (x * 4);
                img->pixels[index].b = pixelData[pos];
                img->pixels[index].g = pixelData[pos + 1];
                img->pixels[index].r = pixelData[pos + 2];
            }
            else if (bpp == 8) {
                int pos = (py * rowSize8) + x;
                int val = pixelData[pos];
                img->pixels[index].r = val;
                img->pixels[index].g = val;
                img->pixels[index].b = val;
            }
            
            img->pixels[index].x = x;
            img->pixels[index].y = y;
            index++;
        }
    }

    printf("Loaded %d-bit BMP: %dx%d pixels! ^w^\n", bpp, width, absHeight);
    
    free(buffer);
    return img;
}

// Don't forget a cleanup function to prevent memory leaks!
void freeImage(openImage *img) {
    if (img != NULL) {
        if (img->pixels != NULL) {
            free(img->pixels);
        }
        free(img);
    }
}



/**
 * Returns a pointer to the pixel at (x, y).
 * Returns NULL if the coordinates are out of bounds! :3
 */
openPixel* getPixel(openImage *img, int x, int y) {
    if (img == NULL || img->pixels == NULL) return NULL;

    // Bounds check to prevent segmention faults >w<
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
        return NULL;
    }

    // Calculate index: (row * width) + column
    int index = (y * img->width) + x;
    return &img->pixels[index];
}
