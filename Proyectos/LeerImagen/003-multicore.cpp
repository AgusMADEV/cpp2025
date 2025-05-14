#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

void process_pixels(
    unsigned char* input_data,
    unsigned char* output_data,
    int channels,
    int start,
    int end
) {
    for (int i = start; i < end; ++i) {
        unsigned char r = input_data[i * channels + 0];
        unsigned char g = input_data[i * channels + 1];
        unsigned char b = input_data[i * channels + 2];

        // Mediana
        unsigned char rgb[] = { r, g, b };
        std::sort(rgb, rgb + 3);
        output_data[i] = rgb[1]; // valor medio
    }
}

int main() {
    using clock = std::chrono::high_resolution_clock;
    auto start_time = clock::now();

    int width, height, channels;
    unsigned char* input_data = stbi_load("image.jpg", &width, &height, &channels, 0);
    if (!input_data) {
        std::cerr << "Error al cargar la imagen.\n";
        return 1;
    }

    if (channels < 3) {
        std::cerr << "La imagen debe tener al menos 3 canales (RGB).\n";
        stbi_image_free(input_data);
        return 1;
    }

    int total_pixels = width * height;
    std::vector<unsigned char> gray_data(total_pixels);

    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback

    std::vector<std::thread> threads;
    int pixels_per_thread = total_pixels / num_threads;

    // Lanzar hilos
    for (int t = 0; t < num_threads; ++t) {
        int start_idx = t * pixels_per_thread;
        int end_idx = (t == num_threads - 1) ? total_pixels : start_idx + pixels_per_thread;

        threads.emplace_back(process_pixels, input_data, gray_data.data(), channels, start_idx, end_idx);
    }

    // Esperar a que todos los hilos terminen
    for (auto& t : threads) {
        t.join();
    }

    // Escribir imagen de salida
    if (!stbi_write_jpg("imagenfinal.jpg", width, height, 1, gray_data.data(), 100)) {
        std::cerr << "Error al guardar la imagen.\n";
        stbi_image_free(input_data);
        return 1;
    }

    stbi_image_free(input_data);

    auto end_time = clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Imagen 'imagenfinal.jpg' generada con éxito.\n";
    std::cout << "Tiempo transcurrido: " << elapsed.count() << " segundos usando " << num_threads << " hilos.\n";

    return 0;
}
