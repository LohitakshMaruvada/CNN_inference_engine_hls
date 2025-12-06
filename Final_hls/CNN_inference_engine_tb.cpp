#include <iostream>
#include <fstream>
#include <cmath>
#include "main.h"  // Your inference engine

typedef ap_fixed<16,8> data_t;

// ==========================
// ResNet-50 Layer Schedule
// ==========================
#define RES50_NUM_LAYERS 50
#define RES50_FC_LAYER 49

static layer_cfg_t cfg[RES50_NUM_LAYERS] = {

    // Example =====================================================
    // NOTE: nl = number of compute outputs in that layer
    //       rl = 1 + (#values per compute)
    // -------------------------------------------------------------
    { CONV, 112*112*64, 3*7*7*1 + 1 },   // Conv1
    { MAXPOOL, 56*56*64, 3*3 },          // MaxPool
    { CONV, 56*56*256, 64*1*1 + 1 },     // Stage2 Block1 Conv A
    { RELU,  56*56*256, 1 },
    // ...
    // ...
    // Repeat for 50 layers
    // ...
    { FC, 1000, 2048 + 1 }              // Final Layer - FC1000
};

// ==========================
// Testbench
// ==========================
int main() {

    std::cout << "\nRunning ResNet-50 HLS Testbench...\n";

    const int STREAM_SIZE = 50 * 1024 * 1024;
    data_t *input_stream = new data_t[STREAM_SIZE];

    std::ifstream fimg("input.bin", std::ios::binary);
    fimg.read((char*)input_stream, STREAM_SIZE * sizeof(data_t));
    fimg.close();

    static data_t class_scores[MAX_CLASSES];  // <-- HERE
    int out_class;
    data_t out_prob;

    CNN_Inference_Top(
        cfg,
        RES50_FC_LAYER,
        RES50_NUM_LAYERS,
        input_stream,
        STREAM_SIZE,
        class_scores,   // <-- REQUIRED
        &out_class,
        &out_prob
    );

    std::cout << "\n===== PREDICTION RESULT =====\n";
    std::cout << "Top-1 Class: " << out_class << "\n";
    std::cout << "Confidence: " << (float)out_prob << "\n";
    std::cout << "=============================\n";

    delete[] input_stream;
    return 0;
}
