
#include <iostream>
#include <cmath>
#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;
typedef ap_fixed<40,16> acc_t;

void compute_conv(
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
) {
    // Output dimensions
    int out_h = (height + 2*padding - kernel_size) / stride + 1;
    int out_w = (width  + 2*padding - kernel_size) / stride + 1;

    if (out_h <= 0 || out_w <= 0) return;

    typedef ap_fixed<40,16> acc_t;   // wide accumulator

Batch_Loop:
    for (int n = 0; n < batch_size; ++n) {
    OutC_Loop:
        for (int oc = 0; oc < out_channels; ++oc) {
        OutH_Loop:
            for (int oh = 0; oh < out_h; ++oh) {
            OutW_Loop:
                for (int ow = 0; ow < out_w; ++ow) {
#pragma HLS PIPELINE II=1

                    acc_t acc = (acc_t)biases[oc];

                    int h_start = oh * stride - padding;
                    int w_start = ow * stride - padding;

                InC_Loop:
                    for (int ic = 0; ic < in_channels; ++ic) {
                    KH_Loop:
                        for (int kh = 0; kh < kernel_size; ++kh) {
                        KW_Loop:
                            for (int kw = 0; kw < kernel_size; ++kw) {

                                int ih = h_start + kh;
                                int iw = w_start + kw;

                                // padding check
                                if (ih < 0 || ih >= height) continue;
                                if (iw < 0 || iw >= width)  continue;

                                // input index
                                int in_idx =
                                    ((n * in_channels + ic) * height + ih) * width + iw;

                                data_t in_val = input[in_idx];

                                // weight index
                                int w_idx =
                                    (((oc * in_channels + ic) * kernel_size + kh)
                                        * kernel_size) + kw;

                                data_t w_val = weights[w_idx];

                                acc += (acc_t)in_val * (acc_t)w_val;
                            }
                        }
                    }

                    // write output
                    int out_idx =
                        ((n * out_channels + oc) * out_h + oh) * out_w + ow;

                    output[out_idx] = (data_t)acc;
                }
            }
        }
    }
}
