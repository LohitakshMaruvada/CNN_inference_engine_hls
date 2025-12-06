#include <ap_int.h>
#include <ap_fixed.h>
#include <limits>
#define MAX_LAYERS 64
#define MAX_CLASSES 1024

typedef ap_fixed<16,8> data_t;
typedef ap_fixed<40,16> acc_t;


// Example computational kernels (dummy placeholders)
data_t compute_relu(data_t in)  { return (in > 0) ? in : (data_t)0; }
data_t compute_relu6(data_t in) {
    if (in <= 0) return (data_t)0;
    else if (in >= 6) return (data_t)6;
    else return in;
}

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
) {
    // compute output dimensions
    int out_h = (height + 2*padding - kernel_size) / stride + 1;
    int out_w = (width  + 2*padding - kernel_size) / stride + 1;

    // Safety: if out dims non-positive, nothing to do
    if (out_h <= 0 || out_w <= 0) return;

    // A very small value for initialization. Using numeric_limits for portability.
    // For ap_fixed this should work; if your toolchain complains, replace with a constant (e.g., (data_t)-1e6).
    const data_t NEG_INF = (data_t) (std::numeric_limits<float>::lowest() / 2.0f);

Batch_Loop:
    for (int b = 0; b < batch_size; ++b) {
    Channel_Loop:
        for (int c = 0; c < channels; ++c) {
        OutH_Loop:
            for (int oh = 0; oh < out_h; ++oh) {
            OutW_Loop:
                for (int ow = 0; ow < out_w; ++ow) {
                    #pragma HLS PIPELINE II=1
                    data_t max_val = NEG_INF;
                    bool any_valid = false;

                    // compute window top-left in input coordinates (taking padding into account)
                    int h_start = oh * stride - padding;
                    int w_start = ow * stride - padding;
                    int h_end = h_start + kernel_size;
                    int w_end = w_start + kernel_size;

                    // iterate over kernel window
                    for (int kh = h_start; kh < h_end; ++kh) {
                        for (int kw = w_start; kw < w_end; ++kw) {

                            // Check bounds
                            if (kh < 0 || kh >= height) continue;
                            if (kw < 0 || kw >= width)  continue;

                            any_valid = true;

                            // compute input index: ((b * channels + c) * height + kh) * width + kw
                            int in_idx = ((b * channels + c) * height + kh) * width + kw;
                            data_t val = input[in_idx];

                            // max operation
                            if (val > max_val) {
                                max_val = val;
                            }
                        }
                    } // kernel loops

                    // if no valid elements (extremely rare), set to 0
                    if (!any_valid) max_val = (data_t)0;

                    // store result: output index ((b*channels + c) * out_h + oh) * out_w + ow
                    int out_idx = ((b * channels + c) * out_h + oh) * out_w + ow;
                    output[out_idx] = max_val;
                } // OutW_Loop
            } // OutH_Loop
        } // Channel_Loop
    } // Batch_Loop
}


