`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 22:09:12
// Design Name: 
// Module Name: mac
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


module mac #(
    parameter DATA_W = 16,
    parameter ACC_W  = 16     // >= 2 * DATA_W recommended
)(
    input  wire clk,
    input  wire rst_n,

    input  wire signed [DATA_W-1:0] in_data,
    input  wire signed [DATA_W-1:0] weight,
    input  wire signed [DATA_W-1:0] psum,
//    input  wire signed [DATA_W-1:0] bias,

    input  wire mac_en,       // from SZD
    input  wire acc_clear,    // from KPC

    output wire signed [ACC_W-1:0] acc_out
//    output wire signed [ACC_W-1:0] prod_out
);

    reg signed [ACC_W-1:0] acc_reg;

    // Multiply result (combinational)
//    assign prod_out = in_data * weight;
    reg [ACC_W-1:0] prod_out;
    // Accumulate
//    always @(posedge clk or negedge rst_n) begin
//        if(!rst_n)
//            acc_reg <= 0;
//        else if(acc_clear)
//            acc_reg <= 0;
//        else if(mac_en)
//            acc_reg <= acc_reg + prod_out;
//        else
//            acc_reg <= acc_reg; // hold
//    end

    always @(posedge clk or negedge rst_n) begin
        if(!rst_n)
            acc_reg <= 0;
        else if(mac_en)
            prod_out <= in_data*weight;
        else
            prod_out <= in_data; // hold
    end


    assign acc_out = prod_out + psum;

endmodule
