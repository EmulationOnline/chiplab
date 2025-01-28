module top 
    # (
        parameter DISPLAY_PINS = 2)
    (
	input  CLK,
	output [DISPLAY_PINS-1:0] OUTPUT
);

    // Expose counter after dropping rightmost bits.
    // This is used as a divider of CLK.
    // When running at 1<<20 aka 1048576 aka 1.048 mHz,
    // dropping the 20 rightmost bits gives an output that increments
    // once per second.
	localparam LOG2DELAY = 20;

	reg [1+LOG2DELAY:0] counter = 0;
	reg outcnt;
    reg [DISPLAY_PINS-1:0] display = 0;

	always @(posedge CLK) begin
		counter <= counter + 1;
		outcnt <= counter[LOG2DELAY];
        // lsb of outcnt controls shifting.
        // when transitioning to 1, the OUTPUT reg is also shifted. 
	end
    always @(posedge outcnt) begin
        if (display  == 0) begin
            display <= 1;
        end else begin
            display <= (display << 1);
        end
    end

	assign OUTPUT = display;
endmodule
