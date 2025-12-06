#include <ap_int.h>
#include <ap_fixed.h>
#include <limits>
#define MAX_LAYERS 64
#define MAX_CLASSES 1024

typedef ap_fixed<16,8> data_t;
typedef ap_fixed<40,16> acc_t;


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
) {
    // compute output dimensions (same formula as maxpool)
    int out_h = (height + 2*padding - kernel_size) / stride + 1;
    int out_w = (width  + 2*padding - kernel_size) / stride + 1;

    // Safety: if out dims non-positive, nothing to do
    if (out_h <= 0 || out_w <= 0) return;

    // Wider accumulator to avoid overflow during summation; tune width if necessary.
    typedef ap_fixed<40,16> acc_t;

Batch_Loop:
    for (int b = 0; b < batch_size; ++b) {
    Channel_Loop:
        for (int c = 0; c < channels; ++c) {
        OutH_Loop:
            for (int oh = 0; oh < out_h; ++oh) {
            OutW_Loop:
                for (int ow = 0; ow < out_w; ++ow) {
                    #pragma HLS PIPELINE II=1
                    acc_t acc = (acc_t)0;
                    int valid_count = 0;

                    // compute window top-left in input coordinates (taking padding into account)
                    int h_start = oh * stride - padding;
                    int w_start = ow * stride - padding;
                    int h_end = h_start + kernel_size;
                    int w_end = w_start + kernel_size;

                    // iterate over kernel window
                KH_LOOP:
                    for (int kh = h_start; kh < h_end; ++kh) {
                        #pragma HLS LOOP_TRIPCOUNT min=1 max=8
                    KW_LOOP:
                        for (int kw = w_start; kw < w_end; ++kw) {
                            #pragma HLS LOOP_TRIPCOUNT min=1 max=8

                            // Check bounds and skip padded positions
                            if (kh < 0 || kh >= height) continue;
                            if (kw < 0 || kw >= width)  continue;

                            // compute input index: ((b * channels + c) * height + kh) * width + kw
                            int in_idx = ((b * channels + c) * height + kh) * width + kw;
                            data_t val = input[in_idx];

                            // accumulate
                            acc += (acc_t)val;
                            valid_count++;
                        }
                    } // kernel loops

                    data_t out_val;
                    if (valid_count > 0) {
                        // compute average as acc / valid_count
                        out_val = (data_t)(acc / (acc_t)valid_count);
                    } else {
                        // window had no valid elements (very rare) -> set 0
                        out_val = (data_t)0;
                    }

                    // store result: output index ((b*channels + c) * out_h + oh) * out_w + ow
                    int out_idx = ((b * channels + c) * out_h + oh) * out_w + ow;
                    output[out_idx] = out_val;
                } // OutW_Loop
            } // OutH_Loop
        } // Channel_Loop
    } // Batch_Loop
}
