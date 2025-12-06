`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 30.11.2025 00:43:27
// Design Name: 
// Module Name: memory_tb
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

module memory_tb;

    parameter DATA_W = 16;
    parameter W_DEPTH = 9;

    reg clk;
    reg wr_en;
    reg rd_en;
    reg [$clog2(W_DEPTH)-1:0] wr_addr;
    reg [$clog2(W_DEPTH)-1:0] rd_addr;
//    reg [DATA_W-1:0] wr_addr;
    reg signed [DATA_W-1:0] wr_data;
//    reg [DATA_W-1:0] rd_addr;

    wire signed [DATA_W-1:0] rd_data;

    // DUT instantiation
    memory #(
        .DATA_W(DATA_W),
        .W_DEPTH(W_DEPTH)
    ) dut (
        .clk(clk),
        .wr_en(wr_en),
        .rd_en(rd_en),
        .wr_addr(wr_addr),
        .wr_data(wr_data),
        .rd_addr(rd_addr),
        .rd_data(rd_data)
    );

    // clock: 10ns period
    always #5 clk = ~clk;

    initial begin
        clk = 0;
        wr_en = 0;
        rd_en = 0;
        wr_addr = 0;
        wr_data = 0;
        rd_addr = 0;

        $display("\n===== START MEMORY TEST =====\n");

        // Write known values in memory
        @(posedge clk);
        wr_en = 1;
        wr_addr = 0; wr_data = 10;
        @(posedge clk);
        wr_addr = 1; wr_data = 20;
        @(posedge clk);
        wr_addr = 2; wr_data = 30;
        @(posedge clk);
        wr_addr = 3; wr_data = 40;
        @(posedge clk);
        wr_addr = 4; wr_data = 50;
        @(posedge clk);
        wr_addr = 5; wr_data = 60;
        @(posedge clk);
        wr_addr = 6; wr_data = 70;
        @(posedge clk);
        wr_addr = 7; wr_data = 80;
        @(posedge clk);
        wr_addr = 8; wr_data = 90;
        @(posedge clk);

        wr_en = 0;

        // Read back values
        rd_en = 1;
        $display("Time | rd_addr | rd_data");

        rd_addr = 0;
        repeat(9) begin
            @(posedge clk);
            $display("%4t | %7d | %7d", $time, rd_addr, rd_data);
            rd_addr = rd_addr + 1;
        end

        $display("\n===== END MEMORY TEST =====\n");
        $finish;
    end

endmodule

