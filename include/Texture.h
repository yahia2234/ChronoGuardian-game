#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture {
public:
    GLuint ID;
    int width, height, channels;
    std::string path;

    Texture(const char* path, bool generateMipmaps = true);
    Texture(unsigned char* data, int width, int height, int channels); // For procedural textures
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    // Create procedural textures
    static Texture* createCheckerboard(int size, int checkSize);
    static Texture* createNoise(int size);
    static Texture* createSolidColor(unsigned char r, unsigned char g, unsigned char b);

private:
    void loadFromFile(const char* path, bool generateMipmaps);
    void createFromData(unsigned char* data, int w, int h, int ch, bool generateMipmaps = true);
};

#endif

