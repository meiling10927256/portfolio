`timescale 1ns/1ns
module MUX4to1( ALUOut, HiOut, LoOut, Shifter, Signal, dataOut );
input [31:0] ALUOut ;
input [31:0] HiOut ;
input [31:0] LoOut ;
input [31:0] Shifter ;
input [5:0] Signal ;
output [31:0] dataOut ;

wire [31:0] temp ;

// alu
parameter AND = 6'b100100; // 36
parameter OR  = 6'b100101; // 37
parameter ADD = 6'b100000; // 32
parameter SUB = 6'b100010; // 34
parameter SLT = 6'b101010; // 42
// shifter
parameter SLL = 6'b000000; // 0
// divider
parameter DIVU= 6'b011011; // 27
parameter MFHI= 6'b010000; // 16
parameter MFLO= 6'b010010; // 18


assign temp = ( Signal == MFHI ) ? HiOut :( Signal == MFLO ) ? LoOut : ( Signal == SLL ) ? Shifter : ( Signal == AND || OR || ADD || SUB || SLT ) ? ALUOut : 32'b0 ;

assign dataOut = temp ;

endmodule