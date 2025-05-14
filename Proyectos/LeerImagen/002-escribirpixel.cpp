#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

int main() {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    int width, height, channels;
    unsigned char* input_data = stbi_load("image.jpg", &width, &height, &channels, 0);
    if (!input_data) {
        std::cerr << "Failed to load image.\n";
        return 1;
    }

    std::cout << "Loaded image: " << width << "x" << height << ", channels: " << channels << "\n";

    std::vector<unsigned char> grayscale(width * height);

    for (int i = 0; i < width * height; ++i) {
        unsigned char r = input_data[i * channels + 0];
        unsigned char g = input_data[i * channels + 1];
        unsigned char b = input_data[i * channels + 2];

        // Median grayscale
        unsigned char rgb[] = { r, g, b };
        std::sort(rgb, rgb + 3);
        grayscale[i] = rgb[1];  // median
    }

    // Write grayscale image (1 channel)
    if (!stbi_write_jpg("imagenfinal.jpg", width, height, 1, grayscale.data(), 100)) {
        std::cerr << "Failed to write grayscale image.\n";
        stbi_image_free(input_data);
        return 1;
    }

    stbi_image_free(input_data);

    auto end = clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Grayscale image written to 'imagenfinal.jpg'.\n";
    std::cout << "Elapsed time: " << elapsed.count() << " seconds.\n";

    return 0;
}
