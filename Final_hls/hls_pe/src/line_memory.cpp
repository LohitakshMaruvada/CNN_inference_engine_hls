#include <ap_int.h>
#include <ap_fixed.h>

typedef ap_fixed<16,8> data_t;

void line_memory(
        data_t mem[1024],   // A row buffer
        data_t in_val,
        bool write_en,
        int write_addr,
        int read_addr,
        data_t *out_val
){
#pragma HLS PIPELINE II=1

    if(write_en) {
        mem[write_addr] = in_val;
    }

    *out_val = mem[read_addr];
}

