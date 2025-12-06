`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 23:34:02
// Design Name: 
// Module Name: agu_tb
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

module agu_tb;

    parameter W_DEPTH = 16;
    
    reg clk;
    reg rst_n;
    reg rd_en;
    reg wr_en;
    reg [$clog2(W_DEPTH)-1:0] wr_addr;

    wire [$clog2(W_DEPTH)-1:0] rd_addr;
    wire stride_req;

    // DUT instantiation
    agu #(
        .W_DEPTH(W_DEPTH)
    ) uut (
        .clk(clk),
        .rst_n(rst_n),
        .rd_en(rd_en),
        .wr_en(wr_en),
        .wr_addr(wr_addr),
        .rd_addr(rd_addr),
        .stride_req(stride_req)
    );

    // Clock generation (10ns period ? 100MHz)
    always #5 clk = ~clk;

    initial begin
        // Initial conditions
        clk     = 0;
        rst_n   = 0;
        rd_en   = 0;
        wr_en   = 0;
        wr_addr = 0;

        // Release reset after 20ns
        #20 rst_n = 1;

        // Start reading
        #10 rd_en = 1;

        $display("\n====== AGU TEST START ======\n");
        $display("Time | rd_addr | stride_req");

        repeat(W_DEPTH * 2) begin
            @(posedge clk);
            $display("%4t | %7d | %b", $time, rd_addr, stride_req);
        end

        $display("\n====== AGU TEST END ======\n");
        $finish;
    end

endmodule

