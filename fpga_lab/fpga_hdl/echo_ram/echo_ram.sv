module top (
    input CLK,
    input RESET,
    output RW,  // 0 for read
    inout[7:0] DATA
);
    echo big(
        .clk(CLK),
        .reset(RESET),
        .rw(RW),
        .data(DATA),
        .ctr()
    );
endmodule : top

// This is a smaller instance of echo, for faster testing.
module tinytest (
    input clk,
    input reset,
    output rw,  // 0 for read
    inout[7:0] data,
    output[8:0] ctr
);
    echo #(.MEM_BITS(8)) tiny (
        .*
    );
endmodule : tinytest

// Support linear reading/writing from the ram module.
// On reset, the index will be reset. On each clock cycle
// the position steps forward.
// After writing is done, reset and flip rw to read data back.
module echo #(
    MEM_BITS = 16
)(
    input clk,
    input reset,
    output rw,  // 0 to write to memory, 1 means reading out
    inout[7:0] data,
    output[MEM_BITS:0] ctr
);
    logic [7:0] ram [1<<MEM_BITS];

    // The highest bit in counter is used to switch
    // from reading to writing.
    reg [MEM_BITS:0] counter = 0;

    // DEBUG
    assign ctr = counter;

    assign rw = counter[MEM_BITS];
    assign data = rw 
        ? ram[counter[MEM_BITS-1:0]] 
        : 8'hZZ;
    
    always @(posedge clk) begin
        if (reset == 1) begin
            counter <= 0;
        end
        else begin
            counter <= counter + 1;
        end
        if (rw == 0) begin
            ram[counter[MEM_BITS-1:0]] <= data;
        end
    end
    always @(negedge clk) begin
    end
    
    // Trying registers instead of block ram
    // SB_SPRAM256KA ram(
    //     .ADDRESS(next_addr),
    //     .DATAIN(data_in),
    //     .MASKWREN(4'b1111),
    //     .WREN(RW),
    //     .CLOCK(clk),
    //     .CHIPSELECT(1),
    //     .POWEROFF(1),
    //     .STANDBY,
    //     .SLEEP,
    //     .DATAOUT(
    // );
endmodule : echo
