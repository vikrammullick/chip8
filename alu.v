module alu(
    input [7:0] a,
    input [7:0] b,
    input operator_t operator,
    output reg [7:0] out,
    output reg carry 
);
    always@(*) begin  // This is a combinational circuit
        case(operator)
            SET: begin
                out = b;
                carry = 1'b0;
            end
            OR: begin
                out =  a | b;
                carry = 1'b0;
            end
            AND: begin
                out =  a & b;
                carry = 1'b0;
            end
            XOR: begin
                out =  a ^ b;
                carry = 1'b0;
            end
            SUM: begin
                {carry, out} =  a + b;
            end
            SUB: begin
                out =  a - b;
                carry = a >= b ? 1'b1 : 1'b0;
            end
            SHIFT_RIGHT: begin
                out = b >> 1;
                carry = b[0];
            end
            NEG_SUB: begin
                out =  b - a;
                carry = b >= a ? 1'b1 : 1'b0;
            end
            SHIFT_LEFT: begin
                out = b << 1;
                carry = b[7];
            end
            default: begin
                out = 8'd0;
                carry = 1'b0;
            end
        endcase
    end
endmodule
