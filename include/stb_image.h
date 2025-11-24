/*
 * stb_image.h - v2.28 - public domain image loader
 * 
 * This is a single-header library for loading images.
 * Place this file in your include directory.
 * 
 * In ONE C++ file, define STB_IMAGE_IMPLEMENTATION before including:
 * #define STB_IMAGE_IMPLEMENTATION
 * #include "stb_image.h"
 */

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

// Simplified header - full implementation will be in Texture.cpp
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern void stbi_image_free(void *retval_from_stbi_load);
extern void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

#ifdef __cplusplus
}
#endif

// Full stb_image implementation
#ifdef STB_IMAGE_IMPLEMENTATION

// This is a minimal stub - in a real project, you would include the full stb_image.h
// For this project, we'll use procedural textures primarily

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int stbi__vertically_flip_on_load = 0;

void stbi_set_flip_vertically_on_load(int flag) {
    stbi__vertically_flip_on_load = flag;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    // Stub implementation - returns NULL (file loading will fall back to procedural)
    // In a real implementation, this would parse PNG/JPG/etc files
    return NULL;
}

void stbi_image_free(void *data) {
    free(data);
}

#endif // STB_IMAGE_IMPLEMENTATION

#endif // STB_IMAGE_H
