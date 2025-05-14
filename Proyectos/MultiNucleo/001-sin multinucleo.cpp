#include <iostream>
#include <chrono>
#include <cmath>

double heavy_calculation(int iterations) {
    double result = 0.0;
    for (int i = 0; i < iterations; ++i) {
        result += std::sin(i) * std::cos(i); // simulate CPU load
    }
    return result;
}

int main() {
    const int iterations = 500'000'000; // big enough to take some time

    auto start = std::chrono::high_resolution_clock::now();

    double result = heavy_calculation(iterations);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Result: " << result << "\n";
    std::cout << "Time taken (single-threaded): " << elapsed.count() << " seconds\n";

    return 0;
}
