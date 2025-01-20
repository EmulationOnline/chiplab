module top (
	input  CLK,
	output ALWAYS_PIN,
	output CTR0,
	output CTR1,
	output CTR2
);

	localparam BITS = 3;
    // Expose counter after dropping rightmost bits.
    // This is used as a divider of CLK.
    // When running at 1<<20 aka 1048576 aka 1.048 mHz,
    // dropping the 20 rightmost bits gives an output that increments
    // once per second.
	localparam LOG2DELAY = 20;

	reg [BITS+LOG2DELAY:0] counter = 0;
	reg [BITS-1:0] outcnt;

	always @(posedge CLK) begin
		counter <= counter + 1;
		outcnt <= counter[LOG2DELAY+BITS-1:LOG2DELAY];
	end

	assign {CTR2, CTR1, CTR0} = outcnt;
    assign ALWAYS_PIN = 1;
endmodule
