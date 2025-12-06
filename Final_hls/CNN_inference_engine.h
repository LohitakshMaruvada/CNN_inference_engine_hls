#ifndef MAIN_H
#define MAIN_H

#include <ap_fixed.h>
#include <ap_int.h>

typedef ap_fixed<16,8> data_t;
#define MAX_LAYERS 256
#define MAX_CLASSES 1000

typedef enum { CONV, FC, MAXPOOL, AVGPOOL, RELU, RELU6, SKIP_ADD } layer_type_t;

typedef struct {
    layer_type_t type;
    int nl;
    int rl;
} layer_cfg_t;

void CNN_Inference_Top(
    layer_cfg_t cfg[MAX_LAYERS],
    int FClast,
    int nLayers,
    data_t input_data[],
    int input_size,
    data_t output_class_scores[MAX_CLASSES],
    int *out_class,
    data_t *out_value
);

#endif
