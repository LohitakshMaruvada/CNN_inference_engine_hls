`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 21:56:22
// Design Name: 
// Module Name: szd
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

module szd_detector #(
    parameter DATA_W = 16    // fixed16_8 -> 16-bit signed
)(
    input  wire [DATA_W-1:0] in_data,
    input  wire              enable,
    output wire              szd_out     // 1: positive or bypass, 0: negative
);

    // Sign bit: in_data[DATA_W-1]
    assign szd_out = enable ? in_data[DATA_W-1] : 1'b1;

endmodule
