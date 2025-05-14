#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

int main() {
    int width, height, channels;
    unsigned char* data = stbi_load("image.jpg", &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load image.\n";
        return 1;
    }

    std::cout << "Image loaded: " << width << "x" << height << ", channels: " << channels << "\n";

    // First pixel (top-left corner)
    unsigned char r = data[0];
    unsigned char g = data[1];
    unsigned char b = data[2];

    std::cout << "First pixel (RGB): (" << static_cast<int>(r) << ", "
              << static_cast<int>(g) << ", " << static_cast<int>(b) << ")\n";

    stbi_image_free(data);
    return 0;
}
