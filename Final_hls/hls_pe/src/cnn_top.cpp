#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

typedef ap_fixed<16,8> data_t;

void CNN_Top(
        hls::stream<data_t> &input_stream,
        data_t weights[1024],
        data_t bias[1024],
        int nl,
        int rl,
        int FClast,
        int *out_class
){
#pragma HLS INTERFACE axis register both port=input_stream
#pragma HLS INTERFACE m_axi depth=1024 port=weights
#pragma HLS INTERFACE m_axi depth=1024 port=bias
#pragma HLS INTERFACE s_axilite port=nl
#pragma HLS INTERFACE s_axilite port=rl
#pragma HLS INTERFACE s_axilite port=FClast
#pragma HLS INTERFACE s_axilite port=out_class
#pragma HLS INTERFACE s_axilite port=return

    data_t ACmax = 0;
    int CN = 0;

    for(int i = 0; i < nl; i++) {
#pragma HLS PIPELINE II=1

        data_t I = input_stream.read();
        data_t W = weights[i];
        data_t B = bias[i];
        data_t result;

        processing_element(I, W, B,
                           true,   // MAC enabled
                           true,   // ReLU enabled
                           false,  // ReLU6 off
                           false,  // Avg off
                           &result);

        if(result > ACmax){
            ACmax = result;
            CN = i;
        }
    }

    *out_class = CN;
}

