module top(
    input clk, reset
);

    reg [15:0]ADDRESS;

    always @(posedge clk) begin
    end

    SB_SPRAM256KA ram(
        .ADDRESS(ADDRESS),
        .DATAIN(DATA),
        .MASKWREN(4'b1111)
    );

endmodule

