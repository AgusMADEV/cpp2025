#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>

double heavy_calculation(int start, int end) {
    double result = 0.0;
    for (int i = start; i < end; ++i) {
        result += std::sin(i) * std::cos(i);
    }
    return result;
}

int main() {
    const int total_iterations = 500'000'000;
    const int num_threads = std::thread::hardware_concurrency(); // auto-detect core count

    std::cout << "Using " << num_threads << " threads.\n";

    std::vector<std::thread> threads;
    std::vector<double> partial_results(num_threads, 0.0);

    auto start = std::chrono::high_resolution_clock::now();

    int block_size = total_iterations / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int block_start = t * block_size;
        int block_end = (t == num_threads - 1) ? total_iterations : block_start + block_size;

        threads.emplace_back([t, block_start, block_end, &partial_results]() {
            partial_results[t] = heavy_calculation(block_start, block_end);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    double total_result = 0.0;
    for (double partial : partial_results) {
        total_result += partial;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Result: " << total_result << "\n";
    std::cout << "Time taken (multithreaded): " << elapsed.count() << " seconds\n";

    return 0;
}
