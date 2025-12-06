`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02.12.2025 04:14:15
// Design Name: 
// Module Name: pe_v1
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module pe_v1 #(
    parameter DATA_W = 16,
    parameter ACC_W  = 32,
    parameter W_DEPTH = 9,   // z*k weights
    parameter M = 8          // number of input rows (Ix1..IxM)
)(
    input  wire clk,
    input  wire rst_n,

    // Control signals
    input  wire S_ovd,            // override SZD (first layer)
    input  wire Line_sel,         // choose Ix line
    input  wire MAC_EN,           // enable MAC
    input  wire MAX_EN,           // enable MAX block
    input  wire R6_EN,            // enable MIN/ReLU6
    input  wire rd_wr,            // 1 = write weights, 0 = read weights
    input  wire acc_clear,        // reset accumulator
    input  wire max_clear,        // reset MAX block
    input  wire mac_max_sel,      // 0 = MAC/ReLU6, 1 = MAXPOOL

    // Data Ports
    input  wire signed [DATA_W*M-1:0] in_flat,   // {IxM ... Ix2 Ix1}
    input  wire signed [DATA_W-1:0] weight_in,   // for loading
    input  wire signed [ACC_W-1:0]   bias,       // bias or partial sum

    // Outputs
    output wire signed [ACC_W-1:0] psum_out,
    output wire stride_req
);

    // ----------------------------------------
    // Unpack M input data lines (Ix1..IxM)
    // ----------------------------------------
    reg signed [DATA_W-1:0] Ix [0:M-1];
    integer i;

    always @(*) begin
        for (i = 0; i < M; i = i + 1) begin
            Ix[i] = in_flat[i*DATA_W +: DATA_W];
        end
    end

    // Select input line based on Line_sel (for IDM)
    wire signed [DATA_W-1:0] Idm_out;
    assign Idm_out = Ix[Line_sel];

    // ----------------------------------------
    // AGU: Address Generator
    // ----------------------------------------
    wire [$clog2(W_DEPTH)-1:0] addr_w;

    agu #(.W_DEPTH(W_DEPTH)) u_agu (
        .clk(clk),
        .rst_n(rst_n),
        .rd_en(!rd_wr),
        .wr_en(rd_wr),
        .rd_addr(addr_w),
        .stride_req(stride_req)
    );

    // ----------------------------------------
    // Weight Memory (z*k sized)
    // ----------------------------------------
    wire signed [DATA_W-1:0] weight_out;

    memory #(
        .DATA_W(DATA_W),
        .W_DEPTH(W_DEPTH)
    ) u_memory (
        .clk(clk),
        .wr_en(rd_wr),
        .rd_en(!rd_wr),
        .wr_addr(addr_w),
        .wr_data(weight_in),
        .rd_addr(addr_w),
        .rd_data(weight_out)
    );

    // ----------------------------------------
    // Sign Zero Detector (SZD)
    // ----------------------------------------
    wire mac_en_szd;

    szd_detector #(.DATA_W(DATA_W)) u_szd (
        .in_data(Idm_out),
        .enable(S_ovd),
        .szd_out(mac_en_szd)
    );

    wire mac_en_final = MAC_EN & mac_en_szd;

    // ----------------------------------------
    // MAC block
    // ----------------------------------------
    wire signed [ACC_W-1:0] mac_out;
    wire signed [ACC_W-1:0] prod_out;

    mac #(
        .DATA_W(DATA_W),
        .ACC_W(ACC_W)
    ) u_mac (
        .clk(clk),
        .rst_n(rst_n),
        .in_data(Idm_out),
        .weight(weight_out),
        .bias(bias),
        .mac_en(mac_en_final),
        .acc_clear(acc_clear),
        .acc_out(mac_out),
        .prod_out(prod_out)
    );

    // ----------------------------------------
    // MIN (ReLU6)
    // ----------------------------------------
    wire signed [ACC_W-1:0] min_out;

    min6 #(.DATA_W(ACC_W)) u_min6 (
        .in(mac_out),
        .out(min_out)
    );

    // ----------------------------------------
    // MAX (for Maxpool)
    // ----------------------------------------
    wire signed [ACC_W-1:0] max_out;

    max_unit #(.DATA_W(ACC_W)) u_max (
        .clk(clk),
        .rst_n(rst_n),
        .data_in(mac_out),
        .max_en(MAX_EN),
        .max_clear(max_clear),
        .max_out(max_out)
    );

    // ----------------------------------------
    // Output Multiplexer (MAC/ReLU6 OR MAX)
    // ----------------------------------------
    assign psum_out = (mac_max_sel == 1'b0) ? min_out : max_out;

endmodule
