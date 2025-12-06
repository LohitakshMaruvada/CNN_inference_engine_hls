#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;

void minmax(data_t a, data_t b, bool max_sel, data_t *r) {
#pragma HLS INLINE
    if(max_sel)
        *r = (a > b) ? a : b;
    else
        *r = (a < b) ? a : b;
}

