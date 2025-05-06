#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

#define SZ 100000000

// Function to capture output in a file (simulating screenshot)
void capture_output(const std::string& filename, const std::string& content) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << content;
        outfile.close();
        std::cout << "Output saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to save output" << std::endl;
    }
}

// Structure for pthread arguments
typedef struct {
    int* v1;
    int* v2;
    int* v_out;
    int start;
    int end;
} ThreadArgs;

// Thread function for vector addition
void* vector_add_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    for (int i = args->start; i < args->end; ++i) {
        args->v_out[i] = args->v1[i] + args->v2[i];
    }
    return nullptr;
}

// Multithreaded vector addition
void multithreaded_vector_add(int* v1, int* v2, int* v_out, int size) {
    const int num_threads = std::thread::hardware_concurrency();
    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];
    int chunk_size = size / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_args[i].v1 = v1;
        thread_args[i].v2 = v2;
        thread_args[i].v_out = v_out;
        thread_args[i].start = i * chunk_size;
        thread_args[i].end = (i == num_threads - 1) ? size : (i + 1) * chunk_size;
        pthread_create(&threads[i], nullptr, vector_add_thread, &thread_args[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }
}

int main(int argc, char** argv) {
    int size = SZ;
    if (argc > 1) {
        size = std::atoi(argv[1]);
    }

    // Allocate memory
    int* v1 = new int[size];
    int* v2 = new int[size];
    int* v_out = new int[size];

    // Initialize vectors with random values
    std::srand(std::time(nullptr));
    for (int i = 0; i < size; ++i) {
        v1[i] = std::rand() % 100;
        v2[i] = std::rand() % 100;
    }

    // Prepare output string
    std::stringstream ss;
    ss << "Input vectors (first and last 5 elements):\n";
    ss << "v1: ";
    for (int i = 0; i < 5; ++i) ss << v1[i] << " ";
    ss << "... ";
    for (int i = size - 5; i < size; ++i) ss << v1[i] << " ";
    ss << "\nv2: ";
    for (int i = 0; i < 5; ++i) ss << v2[i] << " ";
    ss << "... ";
    for (int i = size - 5; i < size; ++i) ss << v2[i] << " ";

    // Execute multithreaded vector addition
    auto start = std::chrono::high_resolution_clock::now();
    multithreaded_vector_add(v1, v2, v_out, size);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // Output results
    ss << "\n\nMultithreaded output (first and last 5 elements):\n";
    ss << "v_out: ";
    for (int i = 0; i < 5; ++i) ss << v_out[i] << " ";
    ss << "... ";
    for (int i = size - 5; i < size; ++i) ss << v_out[i] << " ";
    ss << "\nMultithreaded execution time: " << elapsed.count() << " ms\n";

    std::cout << ss.str();
    capture_output("multithreaded_output.txt", ss.str());

    delete[] v1;
    delete[] v2;
    delete[] v_out;

    return 0;
}
