#include <iostream>
#include <ap_fixed.h>

// --------------------------------------------------------------
// Match your typedefs
// --------------------------------------------------------------
typedef ap_fixed<16,8> data_t;
typedef ap_fixed<48,24> acc_t;

// --------------------------------------------------------------
// DUT declaration (top function)
// --------------------------------------------------------------
data_t compute_fc();

// Global variables used by compute_fc()
extern const data_t *g_fc_input;
extern const data_t *g_fc_weights;
extern int           g_fc_n;
extern data_t        g_fc_bias;

// --------------------------------------------------------------
// Testbench
// --------------------------------------------------------------
int main() {

    // -------------------------------
    // Setup test vector
    // -------------------------------
    const int n = 4;

    data_t input[n]   = {1, 2, 3, 4};
    data_t weights[n] = {5, 6, 7, 8};
    data_t bias       = 10;

    // Expected:
    // sum = 1*5 + 2*6 + 3*7 + 4*8 = 70
    // result = 70 + 10 = 80
    data_t expected = 80;

    // -------------------------------
    // Initialize globals for DUT
    // -------------------------------
    g_fc_input  = input;
    g_fc_weights = weights;
    g_fc_n      = n;
    g_fc_bias   = bias;

    // -------------------------------
    // Call DUT
    // -------------------------------
    data_t result = compute_fc();

    // -------------------------------
    // Verification
    // -------------------------------
    std::cout << "Result   = " << (float)result   << "\n";
    std::cout << "Expected = " << (float)expected << "\n";

    if (result == expected) {
        std::cout << "TEST PASSED\n";
    } else {
        std::cout << "TEST FAILED\n";
    }

    return 0;
}