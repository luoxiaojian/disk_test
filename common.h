#ifndef COMMON_H_
#define COMMON_H_

#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

struct Vector {
    Vector() : data(nullptr), dim(0) {}

    float* data;
    int dim;
};

// void init_vectors(std::vector<Vector>& vecs, int vec_num, int dim, bool align) {
//     vecs.resize(vec_num);
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<float> dis(0.0f, 100.0f);
//     for (int i = 0; i < vec_num; ++i) {
//         vecs[i].init(dim, align);
//         for (int j = 0; j < dim; ++j) {
//             vecs[i].data[j] = dis(gen);
//         }
//     }
// }

std::vector<int> generate_trace(int query_num, int degree, int vec_num) {
    std::vector<int> trace;
    size_t trace_size = query_num;
    trace_size = trace_size * degree;
    trace.reserve(trace_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, vec_num - 1);
    for (size_t k = 0; k < trace_size; ++k) {
        int cur = dis(gen);
        trace.push_back(cur);
    }
    return trace;
}

template <typename ACCESSOR_T>
void parallel_access(ACCESSOR_T& accessor, int thread_num, int iter) {
    std::vector<std::vector<size_t>> traces(thread_num);
    size_t vec_num = accessor.vec_num();
    int dim = accessor.dim();
    std::cout << "dim = " << dim << ", vec_num = " << vec_num << ", iter = " << iter << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, vec_num - 1);
    for (int i = 0; i < thread_num; ++i) {
        traces[i].reserve(iter);
        for (int j = 0; j < iter; ++j) {
            traces[i].push_back(dis(gen));
        }
    }

    for (int i = 0; i < 10; ++i) {
        std::vector<std::thread> threads;
        std::vector<float> result(thread_num);
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < thread_num; ++j) {
            threads.emplace_back([&](int tid) {
                auto& trace = traces[tid];
                Vector vec;
                accessor.alloc_vec(vec);
                float sum = 0.0;
                for (auto t : trace) {
                    accessor.get(t, vec);
                    for (int k = 0; k < dim; ++k) {
                        sum += vec.data[k];
                    }
                }
                result[tid] = sum;
                accessor.dealloc_vec(vec);
            }, j);
        }
        for (auto& thrd : threads) {
            thrd.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "round - " << i << ": " << static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000000.0 << " s" << std::endl;
        std::cout << "result[0] = " << result[0] << std::endl;
    }
}

#endif // COMMON_H_