`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 22:10:32
// Design Name: 
// Module Name: mac_tb
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

module mac_tb;

    parameter DATA_W = 16;
    parameter ACC_W  = 32;

    reg clk;
    reg rst_n;
    reg signed [DATA_W-1:0] in_data;
    reg signed [DATA_W-1:0] weight;
    reg mac_en;
    reg acc_clear;

    wire signed [ACC_W-1:0] acc_out;
    wire signed [ACC_W-1:0] prod_out;

    // DUT instantiation
    mac #(
        .DATA_W(DATA_W),
        .ACC_W(ACC_W)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .in_data(in_data),
        .weight(weight),
        .mac_en(mac_en),
        .acc_clear(acc_clear),
        .acc_out(acc_out),
        .prod_out(prod_out)
    );

    // Clock generation (100MHz ? 10ns period)
    always #5 clk = ~clk;

    initial begin
        // Initial values
        clk       = 0;
        rst_n     = 0;
        mac_en    = 0;
        acc_clear = 0;
        in_data   = 0;
        weight    = 0;

        // Release reset
        #12 rst_n = 1;

        // Case 1: MAC enabled ? accumulation happens
        in_data = 5; weight = 3; mac_en = 1; acc_clear = 0;
        repeat(3) @(posedge clk);
        #1 $display("T=%0t | Acc1 = %0d (Expected: 45)", $time, acc_out);

        // Case 2: MAC disabled ? accumulator must hold value
        mac_en = 0;
        repeat(2) @(posedge clk);
        #1 $display("T=%0t | Acc2 (hold) = %0d (Expected: 45)", $time, acc_out);

        // Case 3: Clear accumulator
        acc_clear = 1;
        @(posedge clk);
        @(posedge clk);
        acc_clear = 0;
        #1 $display("T=%0t | Acc3 (cleared) = %0d (Expected: 0)", $time, acc_out);

        // Case 4: Test signed handling: (-4)*2 = -8
        mac_en = 1;
        in_data = 4; weight = 2;
        @(posedge clk);
        #1 $display("T=%0t | Acc4 = %0d (Expected: -8)", $time, acc_out);

        $finish;
    end

endmodule
