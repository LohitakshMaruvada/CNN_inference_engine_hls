#include <ap_fixed.h>
#include <ap_int.h>

#define MAX_LAYERS 256
#define MAX_CLASSES 1000
#define MAX_FMAP_SIZE 200000  // tune based on board constraints

typedef ap_fixed<16,8> data_t;
typedef ap_fixed<40,16> acc_t;

typedef enum { CONV, FC, MAXPOOL, AVGPOOL, RELU, RELU6, SKIP_ADD } layer_type_t;

typedef struct {
    layer_type_t type;
    int nl;     // number of output pixels (iterations)
    int rl;     // prefetch words per iteration
} layer_cfg_t;


// ===================================================================
// A – Stream Reader (Conv / FC / Pool) — preserved as requested
// ===================================================================
static data_t *g_stream = nullptr;
static int g_size = 0;
static int g_idx = 0;

static inline data_t stream_read() {
#pragma HLS INLINE
    if(g_idx < g_size)
        return g_stream[g_idx++];
    return (data_t)0;
}


// ===================================================================
// B – Compute Functions — Keep all original functions
// ===================================================================
static data_t compute_conv(int Ktotal) {
#pragma HLS INLINE
    acc_t acc = 0;
    ConvLoop:
    for(int i = 0; i < Ktotal; i++){
#pragma HLS PIPELINE II=1
        data_t a = stream_read();
        data_t w = stream_read();
        acc += (acc_t)a * (acc_t)w;
    }
    acc += (acc_t)stream_read();
    return (data_t)acc;
}

static data_t compute_fc(int n) {
#pragma HLS INLINE
    acc_t acc = 0;
    FCLoop:
    for(int i = 0; i < n; i++){
#pragma HLS PIPELINE II=1
        acc += (acc_t)stream_read() * (acc_t)stream_read();
    }
    acc += (acc_t)stream_read();
    return (data_t)acc;
}

static data_t compute_maxpool(int n) {
#pragma HLS INLINE
    data_t maxv = stream_read();
    MaxLoop:
    for(int i = 1; i < n; i++){
#pragma HLS PIPELINE II=1
        data_t v = stream_read();
        if(v > maxv) maxv = v;
    }
    return maxv;
}

static data_t compute_avgpool(int n) {
#pragma HLS INLINE
    acc_t sum = 0;
    AvgLoop:
    for(int i = 0; i < n; i++){
#pragma HLS PIPELINE II=1
        sum += (acc_t)stream_read();
    }
    return (data_t)(sum / (acc_t)n);
}

static inline data_t do_relu(data_t x){
#pragma HLS INLINE
    return (x > 0) ? x : (data_t)0;
}

static inline data_t do_relu6(data_t x){
#pragma HLS INLINE
    if(x <= 0) return 0;
    if(x >= 6) return 6;
    return x;
}


// ===================================================================
// C – Feature Map Buffers for Skip-Connection Add
// ===================================================================
static data_t FMAP_A[MAX_FMAP_SIZE];
static data_t FMAP_B[MAX_FMAP_SIZE];


// ===================================================================
// MAIN — Complete FSM Execution Unit Fully Preserved
// ===================================================================
void CNN_Inference_Top(
    layer_cfg_t cfg[MAX_LAYERS],
    int FClast,
    int nLayers,
    data_t input_data[],
    int input_size,
    data_t output_class_scores[MAX_CLASSES],
    int *out_class,
    data_t *out_value
){
#pragma HLS INTERFACE m_axi port=input_data  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=output_class_scores offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=cfg        bundle=CTRL
#pragma HLS INTERFACE s_axilite port=FClast     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=nLayers    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=input_size bundle=CTRL
#pragma HLS INTERFACE s_axilite port=out_class  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=out_value  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return     bundle=CTRL

    g_stream = input_data;
    g_size   = input_size;
    g_idx    = 0;

    int l = 0;
    int i = 0;
    int j = 0;

    int write_idx = 0;
    int read_idx = 0;

    data_t ACi = 0;
    data_t ACMax = -999;
    int CN_DC = 0;

MainLoop:
    while(l < nLayers){
        int nl = cfg[l].nl;
        int rl = cfg[l].rl;
        layer_type_t t = cfg[l].type;

        i = j = 0;

    LayerIter:
        while(i < nl){
#pragma HLS PIPELINE II=1

            if(t == SKIP_ADD){
                FMAP_B[write_idx] += FMAP_A[read_idx];
                write_idx++; read_idx++;
                i++; continue;
            }

            if(j < rl){
                FMAP_A[write_idx++] = stream_read();
                j++;
                continue;
            }

            switch(t){
                case CONV:    ACi = compute_conv(rl-1); break;
                case FC:      ACi = compute_fc(rl-1); break;
                case MAXPOOL: ACi = compute_maxpool(rl); break;
                case AVGPOOL: ACi = compute_avgpool(rl); break;
                case RELU:    ACi = do_relu(ACi); break;
                case RELU6:   ACi = do_relu6(ACi); break;
                default: break;
            }

            FMAP_B[write_idx++] = ACi;

            if(l == FClast){
                if(ACi > ACMax){
                    ACMax = ACi;
                    CN_DC = i;
                }
            }

            i++;
            if(i < nl){
                j = 0;
                write_idx = 0;
            }
        }

        if(t != SKIP_ADD){
            for(int k = 0; k < write_idx; k++)
                FMAP_A[k] = FMAP_B[k];
        }

        if(l == FClast){
            *out_class = CN_DC;
            *out_value = ACMax;
            for(int c = 0; c <= CN_DC; c++)
                output_class_scores[c] = FMAP_B[c];
            return;
        }

        write_idx = 0;
        read_idx = 0;
        l++;
    }

    *out_class = CN_DC;
    *out_value = ACMax;
}
