module time_keeper (
    input reg clk,
    input reg reset,
    output reg [7:0] ticks
);
    always @(posedge clk) begin
        if (reset == 1'b1)
            ticks <= 8'd0;
        else if (ticks == 8'd99)
            ticks <= 8'd0; 
        else
            ticks <= ticks + 1;
    end
endmodule
