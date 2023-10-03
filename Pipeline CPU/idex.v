module idex ( clk, rst, en_reg, WB_out, MEM_out, EX_out, shamt_out, funct_out, PC_out, RD1_out, RD2_out, immed_out, rt_out, rd_out, op_out,
								 WB_in,  MEM_in,  EX_in,  shamt_in,  funct_in,  PC_in,  RD1_in,  RD2_in,  immed_in,  rt_in,  rd_in, op_in );
    input clk, rst, en_reg;
	input [1:0]  WB_in;
	input [2:0]  MEM_in;
	input [3:0]  EX_in;
	input [4:0]	 rt_in, rd_in, shamt_in;
	input [5:0]  funct_in, op_in;
    input [31:0] PC_in, RD1_in, RD2_in, immed_in;
	
	output [1:0]  WB_out;
	output [2:0]  MEM_out;
	output [3:0]  EX_out;
	output [4:0]  rt_out, rd_out, shamt_out;
	output [5:0]  funct_out, op_out;
    output [31:0] PC_out, RD1_out, RD2_out, immed_out;
	
	reg	  [1:0]  WB_out;
	reg   [2:0]  MEM_out;
	reg   [3:0]  EX_out;
	reg   [4:0]  rt_out, rd_out, shamt_out;
	reg   [5:0]  funct_out, op_out;
    reg   [31:0] PC_out, RD1_out, RD2_out, immed_out;
   
    always @( posedge clk ) begin
        if ( rst ) begin
		WB_out    <= 2'b0;
		MEM_out   <= 3'b0;
		EX_out	  <= 4'b0;
		shamt_out <= 5'b0;
		funct_out <= 6'b0;
		PC_out    <= 32'b0;
		RD1_out   <= 32'b0;
		RD2_out   <= 32'b0; 
		immed_out <= 32'b0; 
		rt_out    <= 5'b0; 
		rd_out    <= 5'b0;
		op_out    <= 5'b0 ;
	end
        else if ( en_reg ) begin
		WB_out    <=  WB_in;
		MEM_out   <=  MEM_in;
		EX_out	  <=  EX_in;
		shamt_out <=  shamt_in;
		funct_out <=  funct_in;
		PC_out    <=  PC_in;
		RD1_out   <=  RD1_in;
		RD2_out   <=  RD2_in; 
		immed_out <=  immed_in; 
		rt_out    <=  rt_in; 
		rd_out    <=  rd_in;
		op_out    <=  op_in;
	end
    end
endmodule