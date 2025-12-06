`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 29.11.2025 22:30:46
// Design Name: 
// Module Name: max
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


module max#(
    parameter DATA_W = 16
)(
    input wire signed [DATA_W-1:0] in1,
    input wire signed [DATA_W-1:0] in2,
    
    output wire signed [DATA_W-1:0] out
    
    
);

assign out = (in1 > in2) ? in1 : in2;

endmodule
