module agu #(
    parameter W_DEPTH = 16  // this W_DEPTH parameter is equivalent to delta in the architectrue.
)(
    input   clk,
    input   rst_n,

    // Read enable from KPC (weight usage per cycle)
    input   rd_en,

    // Weight load interface
    input   wr_en,
//    input   [$clog2(W_DEPTH)-1:0] wr_addr,

    // Outputs
    output  [$clog2(W_DEPTH)-1:0] rd_addr,
    output  stride_req   // High when last weight used
);

    initial begin
        rd_cnt = 0;
    end

    reg [$clog2(W_DEPTH)-1:0] rd_cnt;

    // Read Address Sequence
    always @(posedge clk) begin
        if(!rst_n) begin
            rd_cnt <= 0;
        end
//        else if(rd_en) begin
        else begin
            if(rd_cnt == W_DEPTH-1) begin
                rd_cnt <= 0;     // Wrap around
            end else begin
                rd_cnt <= rd_cnt + 1;
            end
        end
    end

    // Generate stride request pulse on last weight usage
//    assign stride_req = rd_en && (rd_cnt == W_DEPTH-1);
    assign stride_req = (rd_cnt == W_DEPTH-1);
    assign rd_addr = rd_cnt;

endmodule


