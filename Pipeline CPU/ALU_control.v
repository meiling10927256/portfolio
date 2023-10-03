`timescale 1ns/1ns
module ALUControl( clk, Signal, SignaltoALU, SignaltoSHT, SignaltoDIV, SignaltoMUX );
input clk ;
input [5:0] Signal ;
output [5:0] SignaltoALU ;
output [5:0] SignaltoSHT ;
output [5:0] SignaltoDIV ;
output [5:0] SignaltoMUX ;

reg [5:0] temp ;
reg [6:0] counter ;

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

always@( Signal )
begin
  if ( Signal == DIVU ) // divider
  begin
    counter = 0 ;
  end

end

always@( posedge clk )
begin
  temp = Signal ;
  if ( Signal == DIVU )
  begin

    if ( counter == 32 )
    begin
      temp = 6'b111111 ; // Open HiLo reg for Div
      counter = 0 ;
    end
    counter = counter + 1 ;	
  end

end


assign SignaltoALU = Signal ;
assign SignaltoSHT = Signal ;
assign SignaltoDIV = Signal ;
assign SignaltoMUX = Signal ;


endmodule