#include <ap_fixed.h>
#include <ap_int.h>

typedef ap_fixed<16,8> data_t;

void processing_element(
        data_t I,
        data_t W,
        data_t B,
        bool mac_sel,      // 1 = MAC, 0 = MaxPool
        bool relu_en,
        bool relu6_en,
        bool avg_en,
        data_t *P
){
#pragma HLS INLINE
#pragma HLS PIPELINE II=1

    data_t tmp = 0;

    if(mac_sel) {
        tmp = I * W + B;   // MAC
    } else {
        tmp = (I > W) ? I : W;  // MaxPool
    }

    if(relu_en) {
        tmp = (tmp > 0) ? tmp : (data_t)0;
    }

    if(relu6_en) {
        tmp = (tmp > (data_t)6) ? (data_t)6 : tmp;
    }

    if(avg_en) {
        tmp = tmp >> 2;  // example for 4-element avgpool
    }

    *P = tmp;
}

