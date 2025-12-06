#include <iostream>
#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;

// ----------------------------------------------------------
// DUT declaration (your compute_maxpool function)
// ----------------------------------------------------------
void compute_maxpool(
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

// ----------------------------------------------------------
// Single Test Case
// Input: 4x4, C=1, N=1
// Kernel=2, stride=2, pad=0
//
// Expected Output (2x2):
//  5   7
// 13  15
// ----------------------------------------------------------

int main() {
   const int N = 1, C = 1, H = 4, W = 4;
   const int K = 2, S = 2, P = 0;

   const int OUT_H = (H + 2*P - K) / S + 1;
   const int OUT_W = (W + 2*P - K) / S + 1;

   const int INPUT_SIZE  = N * C * H * W;
   const int OUTPUT_SIZE = N * C * OUT_H * OUT_W;

   data_t input[INPUT_SIZE];
   data_t output[OUTPUT_SIZE];
   data_t expected[OUTPUT_SIZE];

   // Fill input: 0..15
   for (int i = 0; i < INPUT_SIZE; ++i)
       input[i] = (data_t)i;

   // Expected maxpool result
   expected[0] = 5;
   expected[1] = 7;
   expected[2] = 13;
   expected[3] = 15;

   // Run DUT
   compute_maxpool(input, output, H, W, C, N, K, S, P);

   // Compare results
   bool pass = true;
   for (int i = 0; i < OUTPUT_SIZE; ++i) {
       if (output[i] != expected[i]) {
           pass = false;
           std::cout << "Mismatch at index " << i
                     << ": got " << (float)output[i]
                     << ", expected " << (float)expected[i] << "\n";
       }
   }

   if (pass)
       std::cout << "TEST PASSED\n";
   else
       std::cout << "TEST FAILED\n";

   return 0;
}