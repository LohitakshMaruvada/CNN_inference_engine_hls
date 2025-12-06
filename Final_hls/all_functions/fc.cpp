#include <ap_int.h>
#include <ap_fixed.h>
#include <limits>
#define MAX_LAYERS 64
#define MAX_CLASSES 1024

typedef ap_fixed<16,8> data_t;
typedef ap_fixed<40,16> acc_t;


data_t compute_fc_param(
    const data_t *input,    // pointer to input vector I_a
    const data_t *weights,  // pointer to weight vector W_a
    int n,                  // vector length
    data_t bias             // bias B
) {
    #pragma HLS INLINE off
    acc_t acc = 0;

    // accumulate MACs
MAC_LOOP:
    for (int i = 0; i < n; ++i) {
        #pragma HLS PIPELINE II=1
        // cast to accumulator type to avoid intermediate precision loss
        acc += (acc_t)input[i] * (acc_t)weights[i];
    }

    // add bias
    acc += (acc_t)bias;

    // cast back to output type (may truncate/saturate depending on ap_fixed settings)
    data_t out = (data_t)acc;
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Optional no-argument wrapper (only use if your call-site can't be changed)
// You MUST set the globals before calling compute_fc()
////////////////////////////////////////////////////////////////////////////////
const data_t *g_fc_input = nullptr;
const data_t *g_fc_weights = nullptr;
int         g_fc_n = 0;
data_t      g_fc_bias = (data_t)0;

data_t compute_fc() {
    // simple safety check for simulation; in synthesis ensure globals are valid
    if (g_fc_input == nullptr || g_fc_weights == nullptr || g_fc_n <= 0) {
        return (data_t)0;
    }
    return compute_fc_param(g_fc_input, g_fc_weights, g_fc_n, g_fc_bias);
}
