module ifBranch ( opcode, zero, dataA, dataB ) ;

    parameter BEQ = 6'd4;
    parameter BNE = 6'd5;

    input  [5:0] opcode;
    input  [31:0] dataA, dataB;
    output zero ;

    assign zero = ( opcode == BEQ && dataA == dataB ) ? 1'b1 :
		  ( opcode == BNE && dataA != dataB ) ? 1'b1 : 1'b0;
	
endmodule