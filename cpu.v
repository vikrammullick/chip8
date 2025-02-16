typedef enum logic [1:0] {
    FETCH = 2'b00,  
    EXECUTE = 2'b01,
    CPU_IDLE = 2'b10 
} cpu_state_t;

typedef enum logic [1:0] {
    DISABLED = 2'b00,
    READ_ENABLE = 2'b01,  
    WRITE_ENABLE = 2'b10
} control_unit_state_t;

typedef enum logic [3:0] {
    SET = 4'h0,
    OR = 4'h1,
    AND = 4'h2,
    XOR = 4'h3,
    SUM = 4'h4,
    SUB = 4'h5,
    SHIFT_RIGHT = 4'h6,
    NEG_SUB = 4'h7,
    SHIFT_LEFT = 4'hE
} operator_t;

module cpu (
    input reg clk,
    input reg reset,

    input reg[7:0] data_in,
    output reg[7:0] data_out,
    output reg[15:0] addr,
    output control_unit_state_t cu_state,
    output reg[15:0] error,

    output reg[15:0] pc,
    output reg[15:0] op_code,
    output reg[7:0] ticks
);
    wire [3:0] n;
    wire [3:0] x;
    wire [3:0] y;
    wire [7:0] kk;
    wire [11:0] nnn;

    assign x = op_code[11:8];
    assign y = op_code[7:4];
    assign n = op_code[3:0];
    assign kk = op_code[7:0];
    assign nnn = op_code[11:0];

    time_keeper tk (
        .clk(clk),
        .reset(reset),
        .ticks(ticks)
    );
   
    wire [7:0] alu_res;
    wire carry;
    alu al (
      .a(Vx[x]),  
      .b(Vx[y]), 
      .operator(n),
      .out(alu_res),
      .carry(carry)
    );
    
    cpu_state_t cpu_state;
    
    reg [7:0] Vx [15:0];
    reg [15:0] I;

    reg [15:0] sp;

    // TODO: combine?
    reg [7:0] serialize_val;
    reg [7:0] keyboard_lo; 
    
    always @(posedge clk) begin
        if (reset) begin
            cpu_state <= FETCH;
            cu_state <= DISABLED;
            pc <= 16'h0200;
            sp <= 16'd4080; // 4096 - size of stack (16 bytes) 
            addr <= 16'h0000;
            error <= 16'h0;
        end
        else if (ticks == 8'd0) begin
            cpu_state <= FETCH;
            cu_state <= READ_ENABLE;
            addr <= pc;
        end
        else if (ticks == 8'd1) begin
            cpu_state <= FETCH;
            cu_state <= READ_ENABLE;
            addr <= pc + 1;
            // set op code
            op_code[15:8] <= data_in;
        end
        else if (ticks == 8'd2) begin 
            cpu_state <= EXECUTE;
            cu_state <= DISABLED;
            pc <= pc + 2;
            addr <= 16'h0000;
            // set op code
            op_code[7:0] <= data_in;
        end
        else if (cpu_state == EXECUTE) begin
            casez(op_code)
                16'h00E0: begin
                    if (ticks == 8'd3) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= 16'h0005; // PPU_CLEAR_OR_READ_VBLANK_ADDR
                        data_out <= 8'h00;
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= DISABLED;
                        addr <= 16'h0000;
                        data_out <= 8'h00;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h00EE: begin
                    if (ticks == 8'd3) begin
                        cu_state <= READ_ENABLE;
                        addr <= sp - 1;
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= READ_ENABLE;
                        addr <= sp - 2;
                        pc[15:8] <= data_in;
                    end 
                    else if (ticks == 8'd5) begin
                        cu_state <= DISABLED;
                        sp <= sp - 2;
                        pc[7:0] <= data_in;
                        addr <= 16'h0000;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h1???: begin
                    if (ticks == 8'd3) begin
                        pc <= {4'h0, nnn};
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h2???: begin
                    if (ticks == 8'd3) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= sp;
                        data_out <= pc[7:0];
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= sp + 1;
                        data_out <= pc[15:8];
                    end 
                    else if (ticks == 8'd5) begin
                        cu_state <= DISABLED;
                        sp <= sp + 2;
                        data_out <= 8'h00;
                        pc <= {4'h0, nnn};
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h3???: begin
                    if (ticks == 8'd3) begin
                        if (Vx[x] == kk) begin
                            pc <= pc + 2;
                        end
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'h4???: begin
                    if (ticks == 8'd3) begin
                        if (Vx[x] != kk) begin
                            pc <= pc + 2;
                        end
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'h5??0: begin
                    if (ticks == 8'd3) begin
                        if (Vx[x] == Vx[y]) begin
                            pc <= pc + 2;
                        end
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'h9??0: begin
                    if (ticks == 8'd3) begin
                        if (Vx[x] != Vx[y]) begin
                            pc <= pc + 2;
                        end
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'h6???: begin
                    if (ticks == 8'd3) begin
                        Vx[x] <= kk;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h7???: begin
                    if (ticks == 8'd3) begin
                        Vx[x] <= Vx[x] + kk;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'h8???: begin
                    if (ticks == 8'd3) begin
                        Vx[x] <= alu_res;
                        Vx[4'hF] <= {7'b0, carry}; 
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hA???: begin
                    if (ticks == 8'd3) begin
                        I <= {4'h0, nnn};
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hB???: begin
                    if (ticks == 8'd3) begin
                        pc <= {4'h0, nnn} + {8'h00, Vx[4'h0]};
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hC???: begin
                    if (ticks == 8'd3) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h000B; // RNG_ADDR
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= DISABLED;
                        addr <= 16'h0000;
                        Vx[x] <= kk & data_in;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hD???: begin
                    if (ticks == 8'd3) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h0005; // PPU_CLEAR_OR_READ_VBLANK_ADDR
                    end 
                    else if (ticks == 8'd4) begin
                        if (data_in != 8'd0) begin
                            // vblank
                            pc <= pc - 2;
                            cu_state <= DISABLED;
                            addr <= 16'h0000;
                            cpu_state <= CPU_IDLE;
                        end
                        else begin
                            cu_state <= WRITE_ENABLE;
                            addr <= 16'h0006; // PPU_SPRITE_X_ADDR 
                            data_out <= Vx[x];
                        end
                    end 
                    else if (ticks == 8'd5) begin
                        addr <= 16'h0007; // PPU_SPRITE_Y_ADDR 
                        data_out <= Vx[y];
                    end 
                    else if (ticks == 8'd6) begin
                        addr <= 16'h0008; // PPU_SPRITE_ADDR_LO
                        data_out <= I[7:0];
                    end 
                    else if (ticks == 8'd7) begin
                        addr <= 16'h0009; // PPU_SPRITE_ADDR_HI
                        data_out <= I[15:8];
                    end 
                    else if (ticks == 8'd8) begin
                        addr <= 16'h000A; // PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR
                        data_out <= {4'h0, n};
                    end 
                    else if (ticks == 8'd9) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h000A; // PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR
                    end 
                    else if (ticks == 8'd10) begin
                        cu_state <= DISABLED;
                        Vx[4'hF] <= data_in;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hE???: begin
                    if (ticks == 8'd3) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h0000; // KEYBOARD_ADDR_LO
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h0001; // KEYBOARD_ADDR_HI
                        keyboard_lo <= data_in;
                    end 
                    else if (ticks == 8'd5) begin
                        if (({data_in, keyboard_lo} & 16'b1 << Vx[x]) != 16'b0) begin
                            if (kk == 8'h9E) begin
                                pc <= pc + 2;
                            end
                        end
                        else begin
                            if (kk == 8'hA1) begin
                                pc <= pc + 2;
                            end
                        end

                        cu_state <= DISABLED;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hF?07: begin
                    if (ticks == 8'd3) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h0003; // DELAY_TIMER_ADDR
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= DISABLED;
                        addr <= 16'h0000;
                        Vx[x] <= data_in;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hF?0A: begin
                    if (ticks == 8'd3) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= 16'h0002; // KEYBOARD_WAIT_REL_ADDR
                        data_out <= 8'h0;
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= READ_ENABLE;
                        addr <= 16'h0002; // KEYBOARD_WAIT_REL_ADDR
                    end 
                    else if (ticks == 8'd5) begin
                        cu_state <= DISABLED;
                        if (data_in == 8'hFF) begin
                            pc <= pc - 2;
                        end
                        else begin
                            Vx[x] <= data_in;
                        end
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hF?15: begin
                    if (ticks == 8'd3) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= 16'h0003; // DELAY_TIMER_ADDR
                        data_out <= Vx[x];
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= DISABLED;
                        addr <= 16'h0000;
                        data_out <= 8'h00;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hF?18: begin
                    if (ticks == 8'd3) begin
                        cu_state <= WRITE_ENABLE;
                        addr <= 16'h0004; // SOUND_TIMER_ADDR
                        data_out <= Vx[x];
                    end 
                    else if (ticks == 8'd4) begin
                        cu_state <= DISABLED;
                        addr <= 16'h0000;
                        data_out <= 8'h00;
                        cpu_state <= CPU_IDLE;
                    end 
                end
                16'hF?1E: begin
                    if (ticks == 8'd3) begin
                        I <= I + {8'h0, Vx[x]};
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'hF?29: begin
                    if (ticks == 8'd3) begin
                        I <= {8'h0, Vx[x]} * 16'h05 + 16'h50; // FONT SIZE AND FONT OFFSET 
                        cpu_state <= CPU_IDLE;
                    end
                end
                16'hF?33: begin
                    if (ticks == 8'd3) begin
                        serialize_val <= Vx[x] / 10;
                        
                        cu_state <= WRITE_ENABLE;
                        data_out <= Vx[x] % 10;
                        addr <= I + 2;
                    end
                    if (ticks == 8'd4) begin
                        serialize_val <= serialize_val / 10;
                        
                        cu_state <= WRITE_ENABLE;
                        data_out <= serialize_val % 10;
                        addr <= I + 1;
                    end
                    if (ticks == 8'd5) begin
                        cu_state <= WRITE_ENABLE;
                        data_out <= serialize_val % 10;
                        addr <= I;
                    end
                    if (ticks == 8'd6) begin
                        cu_state <= DISABLED;
                        cpu_state <= CPU_IDLE;
                    end
                end 
                16'hF?55: begin
                    if (ticks <= {4'd0, x} + 8'd4) begin
                        if (ticks == {4'd0, x} + 8'd4) begin  
                            cu_state <= DISABLED;
                            cpu_state <= CPU_IDLE;
                        end
                        else begin
                            cu_state <= WRITE_ENABLE;
                            data_out <= Vx[ticks[3:0] - 4'd3];
                            addr <= I;
                            I <= I + 1;
                        end
                    end
                end 
                16'hF?65: begin
                    if (ticks <= {4'd0, x} + 8'd4) begin
                        if (ticks != 8'd3) begin
                            Vx[ticks[3:0] - 4'd4] <= data_in;
                        end
                        
                        if (ticks == {4'd0, x} + 8'd4) begin  
                            cu_state <= DISABLED;
                            cpu_state <= CPU_IDLE;
                        end
                        else begin
                            cu_state <= READ_ENABLE;
                            addr <= I;
                            I <= I + 1;
                        end
                    end
                end 
                default: begin
                    error <= op_code;
                end
            endcase
        end
    end
endmodule
