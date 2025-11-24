#include "Texture.h"
#include <iostream>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* filePath, bool generateMipmaps) : ID(0), width(0), height(0), channels(0), path(filePath) {
    loadFromFile(filePath, generateMipmaps);
}

Texture::Texture(unsigned char* data, int w, int h, int ch) : ID(0), width(w), height(h), channels(ch), path("procedural") {
    createFromData(data, w, h, ch, true);
}

Texture::~Texture() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
}

void Texture::loadFromFile(const char* filePath, bool generateMipmaps) {
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(filePath, &width, &height, &channels, 0);
    
    if (data) {
        createFromData(data, width, height, channels, generateMipmaps);
        stbi_image_free(data);
        std::cout << "Loaded texture: " << filePath << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        // Create a default white texture as fallback
        unsigned char whitePixel[] = {255, 255, 255, 255};
        createFromData(whitePixel, 1, 1, 4, false);
    }
}

void Texture::createFromData(unsigned char* data, int w, int h, int ch, bool generateMipmaps) {
    width = w;
    height = h;
    channels = ch;
    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture* Texture::createCheckerboard(int size, int checkSize) {
    int numChecks = size / checkSize;
    unsigned char* data = new unsigned char[size * size * 3];
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int checkX = x / checkSize;
            int checkY = y / checkSize;
            bool isWhite = (checkX + checkY) % 2 == 0;
            
            int index = (y * size + x) * 3;
            unsigned char color = isWhite ? 220 : 160;
            data[index] = color;
            data[index + 1] = color;
            data[index + 2] = color;
        }
    }
    
    Texture* tex = new Texture(data, size, size, 3);
    delete[] data;
    return tex;
}

Texture* Texture::createNoise(int size) {
    unsigned char* data = new unsigned char[size * size * 3];
    
    for (int i = 0; i < size * size * 3; i++) {
        data[i] = rand() % 256;
    }
    
    Texture* tex = new Texture(data, size, size, 3);
    delete[] data;
    return tex;
}

Texture* Texture::createSolidColor(unsigned char r, unsigned char g, unsigned char b) {
    unsigned char data[] = {r, g, b};
    return new Texture(data, 1, 1, 3);
}

