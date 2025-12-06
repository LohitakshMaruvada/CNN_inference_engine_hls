`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 22:44:20
// Design Name: 
// Module Name: min
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


module min#(
    parameter DATA_W = 16
)(
    input wire signed [DATA_W-1:0] in,
    output wire signed [DATA_W-1:0] out
);

localparam signed [DATA_W-1:0] SIX = 6;

assign out = in[DATA_W-1] ? 0 :   // clamp negative to 0
             (in > SIX)   ? SIX : // clamp above +6
                            in;    // otherwise return input

endmodule
