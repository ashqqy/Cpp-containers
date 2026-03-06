#pragma once

#include <chrono>
#include <random>
#include <vector>

template <typename F>
std::chrono::duration<double, std::milli> MeasureTime(F&& func, std::size_t iterations = 20,
                                                      std::size_t warmup = 2) {
    using clock = std::chrono::steady_clock;

    for (std::size_t i = 0; i < warmup; ++i) {
        std::invoke(std::forward<F>(func));
    }

    std::chrono::duration<double, std::milli> total{0};

    for (std::size_t i = 0; i < iterations; ++i) {
        const auto start = clock::now();
        std::invoke(std::forward<F>(func));
        const auto end = clock::now();

        total += (end - start);
    }

    return total / static_cast<double>(iterations);
}

std::vector<int> GenerateRandomInts(std::size_t n, int from, int to) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(from, to);

    std::vector<int> data;
    data.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
        data.push_back(distrib(gen));
    }

    return data;
}
