`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 23:16:01
// Design Name: 
// Module Name: processing_element
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


module processing_element#(
    parameter DATA_W = 16,
    parameter m = 8, //Line memory length
    parameter W_DEPTH = 4
)(
    input wire clk,
    input wire rst_n,
    input wire S_ovd,
    input wire [$clog2(m)-1:0]Line_selection_Control,
    input wire MAC, //this is MAC?MAX signal
    input wire rd_wr,
    input wire [DATA_W-1:0] weight_in,
    input wire [DATA_W-1:0] bias,  //this can also be partial sum
    input wire [DATA_W*m-1:0] in,
    input wire acc_clear,
    
//    output wire [DATA_W-1:0] psum,
    output wire stride_request,
    output wire [DATA_W-1:0] psum_out
    
);


wire [$clog2(W_DEPTH)-1:0] addr_w;
wire [$clog2(W_DEPTH)-1:0] addr_rd;
wire [DATA_W-1:0] weight_out;
wire szd_out;
wire [DATA_W-1:0] max_in;
wire [DATA_W-1:0] max_out;
wire [DATA_W-1:0] mac_out;
wire [DATA_W-1:0] min_out;
wire [DATA_W-1:0] I_select;


assign I_select = in[(Line_selection_Control*DATA_W) +: DATA_W];
assign max_in = szd_out ? 0 : I_select;
assign psum_out = MAC ? min_out : max_out;

//if rd_wr is 1 then write if rd_wr is zero then read
    agu #(
        .W_DEPTH(W_DEPTH)
    ) wr_agu (
        .clk(clk),
        .rst_n(rst_n),
        .rd_en(!(rd_wr)),
        .wr_en(rd_wr),
        .rd_addr(addr_w),
        .stride_req()
    );
    
    agu #(
        .W_DEPTH(W_DEPTH)
    ) rd_agu (
        .clk(clk),
        .rst_n(rst_n),
        .rd_en(!(rd_wr)),
        .wr_en(rd_wr),
        .rd_addr(addr_rd),
        .stride_req(stride_request)
    );
    
    memory #(
        .DATA_W(DATA_W),
        .W_DEPTH(W_DEPTH)
    ) weight_mem (
        .clk(clk),
        .rst_n(rst_n),
        .wr_en(rd_wr),
        .rd_en(!rd_wr),
        .wr_addr(addr_w),
        .wr_data(weight_in),
        .rd_addr(addr_rd),
        .rd_data(weight_out)
    );

    szd_detector #(
        .DATA_W(DATA_W)
    ) szd_detect(
//        .in_data(I_select),
        .in_data(in[(Line_selection_Control*DATA_W) +: DATA_W]),
        .enable(S_ovd),
        .szd_out(szd_out)
    );
    
    max #(
        .DATA_W(DATA_W)
    ) max_block(
        .in1(max_in),
        .in2(bias),
        .out(max_out)  
    );
    
    mac #(
      .DATA_W(DATA_W),
      .ACC_W(DATA_W)  
    ) mac_block(
        .clk(clk),
        .rst_n(rst_n),
        .in_data(I_select),
        .weight(weight_out),
        .psum(bias),
        .mac_en(!(szd_out)),
        .acc_clear(acc_clear),
        .acc_out(mac_out)
    );
    
    min #(
        .DATA_W(DATA_W)
    ) min_block(
        .in(mac_out),
        .out(min_out)
    );
endmodule
