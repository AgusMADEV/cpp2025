#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>

void print_usage() {
    std::cout << "Uso: exec.exe -i input.jpg -o output.jpg [operacion] [parametro]\n"
              << "Operaciones:\n"
              << "  grayscale\n"
              << "  brightness <valor>\n"
              << "  contrast <valor>\n"
              << "  blackwhite <umbral>\n"
              << "  invert\n";
}

void to_grayscale(unsigned char* data, int w, int h, int c) {
    for (int i = 0; i < w * h; ++i) {
        unsigned char r = data[i * c + 0];
        unsigned char g = data[i * c + 1];
        unsigned char b = data[i * c + 2];
        unsigned char avg = (r + g + b) / 3;
        data[i * c + 0] = data[i * c + 1] = data[i * c + 2] = avg;
    }
}

void adjust_brightness(unsigned char* data, int w, int h, int c, int value) {
    for (int i = 0; i < w * h * c; ++i) {
        int v = data[i] + value;
        data[i] = std::clamp(v, 0, 255);
    }
}

void adjust_contrast(unsigned char* data, int w, int h, int c, float factor) {
    for (int i = 0; i < w * h * c; ++i) {
        int v = static_cast<int>((data[i] - 128) * factor + 128);
        data[i] = std::clamp(v, 0, 255);
    }
}

void to_blackwhite(unsigned char* data, int w, int h, int c, int threshold) {
    for (int i = 0; i < w * h; ++i) {
        unsigned char r = data[i * c + 0];
        unsigned char g = data[i * c + 1];
        unsigned char b = data[i * c + 2];
        unsigned char avg = (r + g + b) / 3;
        unsigned char bw = (avg >= threshold) ? 255 : 0;
        data[i * c + 0] = data[i * c + 1] = data[i * c + 2] = bw;
    }
}

void invert_colors(unsigned char* data, int w, int h, int c) {
    for (int i = 0; i < w * h * c; ++i) {
        data[i] = 255 - data[i];
    }
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        print_usage();
        return 1;
    }

    std::string input_path, output_path, operation;
    int value = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-i" && i + 1 < argc)
            input_path = argv[++i];
        else if (arg == "-o" && i + 1 < argc)
            output_path = argv[++i];
        else if (arg == "grayscale" || arg == "invert") {
            operation = arg;
        } else if (arg == "brightness" || arg == "contrast" || arg == "blackwhite") {
            operation = arg;
            if (i + 1 < argc) value = std::stoi(argv[++i]);
            else {
                std::cerr << "Falta el valor para la operación " << arg << "\n";
                return 1;
            }
        }
    }

    int w, h, c;
    unsigned char* data = stbi_load(input_path.c_str(), &w, &h, &c, 0);
    if (!data) {
        std::cerr << "No se pudo cargar la imagen.\n";
        return 1;
    }

    if (operation == "grayscale")
        to_grayscale(data, w, h, c);
    else if (operation == "brightness")
        adjust_brightness(data, w, h, c, value);
    else if (operation == "contrast")
        adjust_contrast(data, w, h, c, value / 100.0f);
    else if (operation == "blackwhite")
        to_blackwhite(data, w, h, c, value);
    else if (operation == "invert")
        invert_colors(data, w, h, c);
    else {
        std::cerr << "Operación desconocida: " << operation << "\n";
        stbi_image_free(data);
        return 1;
    }

    if (!stbi_write_jpg(output_path.c_str(), w, h, c, data, 100)) {
        std::cerr << "Error al guardar la imagen.\n";
        stbi_image_free(data);
        return 1;
    }

    stbi_image_free(data);
    std::cout << "Imagen procesada y guardada como '" << output_path << "'\n";
    return 0;
}
