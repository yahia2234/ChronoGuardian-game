#include "Texture.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char *filePath, bool generateMipmaps)
    : ID(0), width(0), height(0), channels(0), path(filePath) {
  loadFromFile(filePath, generateMipmaps);
}

Texture::Texture(unsigned char *data, int w, int h, int ch)
    : ID(0), width(w), height(h), channels(ch), path("procedural") {
  createFromData(data, w, h, ch, true);
}

Texture::~Texture() {
  if (ID != 0) {
    glDeleteTextures(1, &ID);
  }
}

void Texture::loadFromFile(const char *filePath, bool generateMipmaps) {
  stbi_set_flip_vertically_on_load(true);

  unsigned char *data = stbi_load(filePath, &width, &height, &channels, 0);

  if (data) {
    createFromData(data, width, height, channels, generateMipmaps);
    stbi_image_free(data);
    std::cout << "Loaded texture: " << filePath << " (" << width << "x"
              << height << ", " << channels << " channels)" << std::endl;
  } else {
    std::cerr << "Failed to load texture: " << filePath << std::endl;
    // Create a default white texture as fallback
    unsigned char whitePixel[] = {255, 255, 255, 255};
    createFromData(whitePixel, 1, 1, 4, false);
  }
}

void Texture::createFromData(unsigned char *data, int w, int h, int ch,
                             bool generateMipmaps) {
  width = w;
  height = h;
  channels = ch;

  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  GLenum format = GL_RGB;
  if (channels == 1)
    format = GL_RED;
  else if (channels == 3)
    format = GL_RGB;
  else if (channels == 4)
    format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);

  if (generateMipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
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

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture *Texture::createCheckerboard(int size, int checkSize) {
  int numChecks = size / checkSize;
  unsigned char *data = new unsigned char[size * size * 3];

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

  Texture *tex = new Texture(data, size, size, 3);
  delete[] data;
  return tex;
}

Texture *Texture::createNoise(int size) {
  unsigned char *data = new unsigned char[size * size * 3];

  for (int i = 0; i < size * size * 3; i++) {
    data[i] = rand() % 256;
  }

  Texture *tex = new Texture(data, size, size, 3);
  delete[] data;
  return tex;
}

Texture *Texture::createSolidColor(unsigned char r, unsigned char g,
                                   unsigned char b) {
  unsigned char data[] = {r, g, b};
  return new Texture(data, 1, 1, 3);
}

Texture *Texture::createCrackedTile(int size) {
  unsigned char *data = new unsigned char[size * size * 3];

  // Base orange color for crumbling tiles
  unsigned char baseR = 230;
  unsigned char baseG = 128;
  unsigned char baseB = 51;

  // Fill with base color
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      int index = (y * size + x) * 3;
      data[index] = baseR;
      data[index + 1] = baseG;
      data[index + 2] = baseB;
    }
  }

  // Add more complex crack patterns - 15 main cracks with branching
  for (int i = 0; i < 15; i++) {
    // Random crack starting point
    int startX = rand() % size;
    int startY = rand() % size;

    // Random crack direction
    float angle = (rand() % 360) * 3.14159f / 180.0f;
    float dx = cos(angle);
    float dy = sin(angle);

    // Draw main crack with varying thickness
    int crackLength = size / 2 + rand() % (size / 2);
    for (int j = 0; j < crackLength; j++) {
      int x = startX + (int)(dx * j);
      int y = startY + (int)(dy * j);

      if (x >= 0 && x < size && y >= 0 && y < size) {
        // Vary crack thickness along length
        int thickness = 1 + (rand() % 2);

        for (int ty = -thickness; ty <= thickness; ty++) {
          for (int tx = -thickness; tx <= thickness; tx++) {
            int nx = x + tx;
            int ny = y + ty;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
              int nindex = (ny * size + nx) * 3;
              // Very dark color for main cracks
              data[nindex] = baseR * 0.2f;
              data[nindex + 1] = baseG * 0.2f;
              data[nindex + 2] = baseB * 0.2f;
            }
          }
        }

        // Add branching cracks (30% chance at each point)
        if (rand() % 100 < 30 && j > 5) {
          float branchAngle = angle + ((rand() % 2 == 0) ? 0.5f : -0.5f);
          float branchDx = cos(branchAngle);
          float branchDy = sin(branchAngle);
          int branchLength = 10 + rand() % 20;

          for (int b = 0; b < branchLength; b++) {
            int bx = x + (int)(branchDx * b);
            int by = y + (int)(branchDy * b);

            if (bx >= 0 && bx < size && by >= 0 && by < size) {
              int bindex = (by * size + bx) * 3;
              data[bindex] = baseR * 0.3f;
              data[bindex + 1] = baseG * 0.3f;
              data[bindex + 2] = baseB * 0.3f;
            }
          }
        }
      }
    }
  }

  // Add fine hairline cracks for more detail
  for (int i = 0; i < 25; i++) {
    int x1 = rand() % size;
    int y1 = rand() % size;
    int x2 = x1 + (rand() % 40) - 20;
    int y2 = y1 + (rand() % 40) - 20;

    // Draw line from (x1,y1) to (x2,y2)
    int steps = std::max(abs(x2 - x1), abs(y2 - y1));
    for (int s = 0; s <= steps; s++) {
      float t = (float)s / steps;
      int x = x1 + (int)((x2 - x1) * t);
      int y = y1 + (int)((y2 - y1) * t);

      if (x >= 0 && x < size && y >= 0 && y < size) {
        int index = (y * size + x) * 3;
        data[index] = data[index] * 0.6f;
        data[index + 1] = data[index + 1] * 0.6f;
        data[index + 2] = data[index + 2] * 0.6f;
      }
    }
  }

  // Add some noise/variation for texture
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      int index = (y * size + x) * 3;
      int variation = (rand() % 30) - 15; // -15 to +15
      data[index] = std::max(0, std::min(255, (int)data[index] + variation));
      data[index + 1] =
          std::max(0, std::min(255, (int)data[index + 1] + variation));
      data[index + 2] =
          std::max(0, std::min(255, (int)data[index + 2] + variation));
    }
  }

  Texture *tex = new Texture(data, size, size, 3);
  delete[] data;
  return tex;
}
