module address_decoder(
    input [15:0] addr,
    output reg [5:0] chip_select
);
    always@(*) begin  // This is a combinational circuit
        case(addr)
            16'h0000, 16'h0001, 16'h0002:
                chip_select = 6'b000010;
            16'h0003:
                chip_select = 6'b000100;
            16'h0004:
                chip_select = 6'b001000;
            16'h0005, 16'h0006, 16'h0007, 16'h0008, 16'h0009, 16'h000A:
                chip_select = 6'b010000;
            16'h000B:
                chip_select = 6'b100000;
            default:
                chip_select = 6'b000001;
        endcase
    end
endmodule
