`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 30.11.2025 00:32:42
// Design Name: 
// Module Name: memory
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


module memory #(
    parameter DATA_W = 16, // this is k
    parameter W_DEPTH = 9  // this is z
)(
    input  wire clk,
    input wire rst_n,

    // Weight write interface
    input  wire wr_en,
    input wire rd_en,
//    input  wire [DATA_W-1:0] wr_addr,
    input  wire signed [DATA_W-1:0] wr_data,

    input wire [$clog2(W_DEPTH)-1:0] wr_addr,
    input wire [$clog2(W_DEPTH)-1:0] rd_addr,

    // Weight read interface (from AGU)
//    input  wire [DATA_W-1:0] rd_addr,
    output reg  signed [DATA_W-1:0] rd_data
);

    // Weight storage RAM
    reg signed [DATA_W-1:0] mem [0:W_DEPTH-1];

    // Write port
    always @(posedge clk) begin
        if(wr_en && rst_n)
            mem[wr_addr] <= wr_data;
    end

    // Read port (synchronous read)
    always @(posedge clk) begin
        if(rd_en && rst_n)
            rd_data <= mem[rd_addr];
        else
            rd_data <= 0;
    end


endmodule

