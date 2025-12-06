#include <ap_int.h>

void AGU(int base, int stride, int iter, int *out_addr) {
#pragma HLS INLINE
    *out_addr = base + (stride * iter);
}

