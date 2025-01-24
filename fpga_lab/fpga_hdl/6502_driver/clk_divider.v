module clk_divider (
    input clk, reset,
    [31:0] divisor,
    output reg divided
);
    // Divides full clock cycles by a given multiple. 
    reg [31:0] DIVISOR;
    reg [31:0] CTR;  // TODO: set to appropriate length based on divisor

    always @(posedge clk) begin
        if (reset) begin
            CTR <= 32'h0;
            DIVISOR <= divisor;
            divided <= 0;
        end
        else if (CTR +1 == DIVISOR) begin
            CTR <= 0;
            divided <= !divided;
        end
        else begin
            CTR <= CTR + 1;
        end
    end

endmodule

