#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <iostream>

std::uint64_t fib(int n) {
    if (n == 1) {
        return 1;
    } else if (n == 2) {
        return 1;
    } else {
        return fib(n-1) + fib(n-2);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Expected 1 argument: Nth fib number" << std::endl;
        return -1;
    }

    int n = std::atoi(argv[1]);
    if (n <= 0) {
        std::cout << "Input " << n << " must be at least 1" << std::endl;
        return -1;
    }
    std::cout << "Computing " << n << "th fib number" << std::endl;

    const auto start = std::chrono::steady_clock::now();
    int64_t fib_num = fib(n);
    const auto end = std::chrono::steady_clock::now();

    std::cout << n << "th fib number is " << fib_num << std::endl;
    const std::chrono::duration<double, std::milli> ms_double = end - start;
    std::cout << "Time: " << ms_double.count() << " ms\n";
}