// tb_compute_conv.cpp
// Minimal testbench for compute_conv (single test case)
// Assumes compute_conv is defined elsewhere and linked into the HLS project.
//
// Test case:
// N=1, Cin=1, Cout=1, H=3, W=3, K=2, S=1, P=0
// Input:
// 0 1 2
// 3 4 5
// 6 7 8
// Kernel (2x2) all ones -> each output is sum of 4 elements
// Expected outputs (out_h=2, out_w=2):
//  8, 12
// 20, 24

#include <iostream>
#include <cmath>
#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;

// DUT declaration (implemention provided in your project sources)
extern void compute_conv(
    const data_t *input,      // N*C*H*W
    data_t *output,           // N*O*out_h*out_w
    int height,
    int width,
    int in_channels,
    int out_channels,
    int batch_size,
    int kernel_size,
    int stride,
    int padding,
    const data_t *weights,    // O*C*K*K
    const data_t *biases      // O
);

int main() {
    const int N = 1;
    const int Cin = 1;
    const int Cout = 1;
    const int H = 3;
    const int W = 3;
    const int K = 2;
    const int S = 1;
    const int P = 0;

    const int OUT_H = (H + 2*P - K) / S + 1; // 2
    const int OUT_W = (W + 2*P - K) / S + 1; // 2

    const int IN_SIZE = N * Cin * H * W;     // 9
    const int OUT_SIZE = N * Cout * OUT_H * OUT_W; // 4
    const int WTS_SIZE = Cout * Cin * K * K; // 4
    const int BIAS_SIZE = Cout;              // 1

    // input 0..8
    data_t input[IN_SIZE];
    for (int i = 0; i < IN_SIZE; ++i) input[i] = (data_t)i;

    // weights all ones
    data_t weights[WTS_SIZE];
    for (int i = 0; i < WTS_SIZE; ++i) weights[i] = (data_t)1;

    // biases zero
    data_t biases[BIAS_SIZE];
    biases[0] = (data_t)0;

    // output buffer
    data_t output[OUT_SIZE];
    for (int i = 0; i < OUT_SIZE; ++i) output[i] = (data_t)0;

    // expected values: {8,12,20,24}
    double expected[OUT_SIZE] = {8.0, 12.0, 20.0, 24.0};

    // Call DUT
    compute_conv(input, output, H, W, Cin, Cout, N, K, S, P, weights, biases);

    // Verify with tolerance
    const double tol = 1e-3;
    bool pass = true;
    std::cout << "compute_conv output:\n";
    for (int i = 0; i < OUT_SIZE; ++i) {
        double got = (double)output[i];
        std::cout << " out[" << i << "] = " << got << "  expected = " << expected[i];
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
