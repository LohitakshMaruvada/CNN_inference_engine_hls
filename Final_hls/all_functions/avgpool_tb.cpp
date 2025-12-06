// tb_compute_avgpool.cpp
// Minimal testbench for compute_avgpool -- single test case
// Single batch (N=1), single channel (C=1), H=4, W=4
// Kernel=2, Stride=2, Padding=0
// Input: 0..15
// Expected pooled outputs (2x2):
// block(0,0): {0,1,4,5}  -> avg = 2.5
// block(0,1): {2,3,6,7}  -> avg = 4.5
// block(1,0): {8,9,12,13} -> avg = 10.5
// block(1,1): {10,11,14,15}-> avg = 12.5
//
// The testbench compares the DUT output to these expected values with a small tolerance.

#include <iostream>
#include <cmath>
#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;

// DUT declaration (link to your implementation)
void compute_avgpool(
    const data_t *input,
    data_t *output,
    int height,
    int width,
    int channels,
    int batch_size,
    int kernel_size,
    int stride,
    int padding
);

int main() {
    // Test parameters
    const int N = 1;
    const int C = 1;
    const int H = 4;
    const int W = 4;
    const int K = 2;
    const int S = 2;
    const int P = 0;

    const int OUT_H = (H + 2*P - K) / S + 1; // 2
    const int OUT_W = (W + 2*P - K) / S + 1; // 2

    const int IN_SIZE  = N * C * H * W;      // 16
    const int OUT_SIZE = N * C * OUT_H * OUT_W; // 4

    // Allocate and fill input (NCHW). Single batch & channel -> simple row-major
    data_t input[IN_SIZE];
    for (int i = 0; i < IN_SIZE; ++i) input[i] = (data_t)i; // 0..15

    // Output buffer
    data_t output[OUT_SIZE];
    for (int i = 0; i < OUT_SIZE; ++i) output[i] = (data_t)0;

    // Expected values (double for tolerant comparison)
    double expected[OUT_SIZE];
    expected[0] = (0 + 1 + 4 + 5) / 4.0;   // 2.5
    expected[1] = (2 + 3 + 6 + 7) / 4.0;   // 4.5
    expected[2] = (8 + 9 + 12 + 13) / 4.0; // 10.5
    expected[3] = (10 + 11 + 14 + 15) / 4.0;//12.5

    // Call DUT
    compute_avgpool(input, output, H, W, C, N, K, S, P);

    // Verify with a small tolerance
    const double tol = 1e-3;
    bool pass = true;

    std::cout << "compute_avgpool output (as double):\n";
    for (int i = 0; i < OUT_SIZE; ++i) {
        double got = (double) output[i];
        std::cout << " out[" << i << "] = " << got << "   expected = " << expected[i];
        if (std::fabs(got - expected[i]) > tol) {
            std::cout << "  <-- MISMATCH";
            pass = false;
        }
        std::cout << "\n";
    }

    if (pass) {
        std::cout << "TEST PASSED\n";
        return 0;
    } else {
        std::cout << "TEST FAILED\n";
        return 1;
    }
}
