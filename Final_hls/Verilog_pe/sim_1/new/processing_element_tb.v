`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 30.11.2025 02:28:04
// Design Name: 
// Module Name: processing_element_tb
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


`timescale 1ns/1ps

`timescale 1ns/1ps

module processing_element_tb;

    parameter DATA_W = 16;
    parameter m = 8;
    parameter W_DEPTH = 4;

    reg clk, rst_n;
    reg S_ovd;
    reg MAC;   // 0 ? MIN(ReLU6), 1 ? MAX
    reg rd_wr; // 1 ? write weights, 0 ? read weights (MAC)
    reg acc_clear;
    reg signed [DATA_W*m-1:0] in;
    reg signed [DATA_W-1:0] weight_in;
    reg signed [DATA_W-1:0] bias;
    reg [$clog2(m)-1:0] Line_selection_Control;

    wire stride_request;
    wire signed [DATA_W-1:0] psum_out;

    integer i;

    // DUT instantiation
    processing_element #(
        .DATA_W(DATA_W),
        .m(m),
        .W_DEPTH(W_DEPTH)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .S_ovd(S_ovd),
        .Line_selection_Control(Line_selection_Control),
        .MAC(MAC),
        .rd_wr(rd_wr),
        .weight_in(weight_in),
        .bias(bias),
        .in(in),
        .acc_clear(acc_clear),
        .stride_request(stride_request),
        .psum_out(psum_out)
    );

    // clock
    always #5 clk = ~clk;

    initial begin
        $display("\n===== PE TEST START =====\n");
        clk = 0;
        rst_n = 0;
        S_ovd = 1; // bypass SZD ? always MAC enabled
        MAC = 0;   // use MIN path initially
        acc_clear = 1;
        rd_wr = 1; // weight load mode
        bias = 0;
        Line_selection_Control = 0;

        // initialize input feature values = 2
        for(i=0;i<m;i=i+1) begin
            in[i*DATA_W +: DATA_W] = 2;
        end

        // assert reset
        @(posedge clk);
        rst_n = 1;

        // ============================
        // Load weights into memory
        // ============================
        @(negedge clk); acc_clear = 0;
        @(negedge clk); weight_in = 10;
        @(negedge clk); weight_in = 20;
        @(negedge clk); weight_in = 30;
        @(negedge clk); weight_in = 40;

        // switch to MAC read mode
        rd_wr = 0;
        acc_clear = 1;
        @(posedge clk);
        acc_clear = 0;

        $display("\n===== READ WEIGHTS / MAC =====\n");
        repeat(8) begin
            @(posedge clk);
            $display("T=%0t | psum_out=%0d | strideReq=%b | w_mode=%b",
                     $time, psum_out, stride_request, rd_wr);
        end

        $display("\n===== PE TEST END =====\n");
        $finish;
    end

endmodule


