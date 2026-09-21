// benchmark.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include <omp.h>

struct AgentSoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> phase;
};

int main() {
    const int N = 1000000;
    const int FRAMES = 10;

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    AgentSoA agents;
    agents.x.resize(N); agents.y.resize(N); agents.z.resize(N);
    agents.vx.resize(N); agents.vy.resize(N); agents.vz.resize(N);
    agents.phase.resize(N);
    for (int i = 0; i < N; ++i) {
        agents.x[i] = dist(rng); agents.y[i] = dist(rng); agents.z[i] = dist(rng);
        agents.vx[i] = dist(rng); agents.vy[i] = dist(rng); agents.vz[i] = dist(rng);
        agents.phase[i] = dist(rng);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int f = 0; f < FRAMES; ++f) {
        #pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            agents.x[i] += agents.vx[i] * 0.016f;
            agents.y[i] += agents.vy[i] * 0.016f;
            agents.z[i] += agents.vz[i] * 0.016f;
            agents.phase[i] += 0.1f;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "SoA + OpenMP: " << diff.count() / FRAMES * 1000.0 << " ms/frame\n";
    std::cout << "Threads used: " << omp_get_max_threads() << "\n";
    return 0;
}
