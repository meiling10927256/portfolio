//	Title: MIPS pipelined Processor

module mips_pipeline( clk, rst );
	input clk, rst;
	
	// instruction bus
	wire[31:0] instr, instr_out;
	
	// break out important fields from instruction
	wire [5:0] opcode, opcode_out, funct, funct_out;
    wire [4:0] rs, rt, rd, shamt, rt_out, rd_out, shamt_out ;
    wire [15:0] immed;
    wire [31:0] extend_immed, extend_immed_un, immed_result, extend_immed_out, b_offset;
    wire [25:0] jumpoffset;
	
	// datapath signals
    wire [4:0] rfile_wn, rfile_wn_out, wn_2;
    wire [31:0] rfile_rd1, rfile_rd2, rfile_wd, rfile_rd1_out, rfile_rd2_out,
				alu_b, alu_out, b_tgt, b_out,
				pc_next, pc, pc_incr, pc_incr_in, pc_incr_out ;
	wire [31:0] rd2ToWD, aluToADDR, ADDR_out,
				dmem_rdata, dmem_rdata_out,
				jump_addr, branch_addr;

	// control signals
    wire RegWrite, Branch, PCSrc, RegDst, MemtoReg, MemRead, MemWrite, ALUSrc, Zero, Jump ,Zero_out,ExtendSel ;
    wire [1:0] ALUOp;
    wire [5:0] Operation;
	
	wire [1:0] WB_reg, WB_reg_1, WB_reg_2, WB_reg_3;
	wire [2:0] MEM_reg, MEM_reg_1, MEM_reg_2;
	wire [3:0] EX_reg, EX_reg_1;
	
    assign opcode = instr_out[31:26];
    assign rs = instr_out[25:21];
    assign rt = instr_out[20:16];
    assign rd = instr_out[15:11];
    assign shamt = instr_out[10:6];
    assign funct = instr_out[5:0];
    assign immed = instr_out[15:0];
    assign jumpoffset = instr_out[25:0];
	

	
	// jump offset shifter & concatenation
	assign jump_addr = { pc_incr[31:28], jumpoffset << 2 };

	assign WB_reg = { MemtoReg, RegWrite };   
	assign MEM_reg = { Branch, MemWrite, MemRead };  
	assign EX_reg = { ALUSrc, RegDst, ALUOp };   // [1:0]是ALUOP

	

	// module instantiations
	// Stage1
	
	// PC
	mux2 #(32) PCMUX( .sel(PCSrc), .a(pc_incr), .b(b_out), .y(branch_addr) );
	// y = ( sel == 0 ) ? a : b
	mux2 #(32) JUMPMUX( .sel(Jump), .a(branch_addr), .b(jump_addr), .y(pc_next) );
	
	reg32 PC( .clk(clk), .rst(rst), .en_reg(1'b1), .d_in(pc_next), .d_out(pc) );
	
	// PC + 4
	TotalALU pcadd( clk, pc, 32'd4, shamt_out, 6'b100000, pc_incr, 1'b0 );
	// TotalALU( clk, dataA, dataB, Shamount, Signal, Output, reset )
	
	// Instruction Mem
	memory InstrMem( .clk(clk), .MemRead(1'b1), .MemWrite(1'b0), .wd(32'd0), .addr(pc), .rd(instr) );
	// 將pc讀到instr
	
	// Stage2
	
	control_unit CTL( .opcode(opcode), .RegDst(RegDst), .ALUSrc(ALUSrc), .MemtoReg(MemtoReg), .RegWrite(RegWrite),
					  .MemRead(MemRead), .MemWrite(MemWrite), .Branch(Branch), .Jump(Jump), .ALUOp(ALUOp), .ExtendSel(ExtendSel) );
  
	reg_file RegFile( .clk(clk), .RegWrite(WB_reg_3[0]), .RN1(rs), .RN2(rt), 
					.WN(wn_2), .WD(rfile_wd), .RD1(rfile_rd1), .RD2(rfile_rd2) );
					
	// sign-extender
	sign_extend SignExt( .immed_in(immed), .ext_immed_out(extend_immed) );	
	unsign_extend unSignExt( .immed_in(immed), .ext_immed_out(extend_immed_un) );
	mux2 #(32) EXMUX( .sel(ExtendSel), .a(extend_immed), .b(extend_immed_un), .y(immed_result) );
	
	// Stage3
	
	// ADD
	// branch offset shifter
    assign b_offset = extend_immed_out << 2;
	TotalALU bradd( clk, pc_incr_out, b_offset, shamt_out, 6'b100000, b_tgt, 1'b0 );
	// TotalALU( clk, dataA, dataB, Shamount, Signal, Output, reset )
	
	// ALU
	mux2 #(32) ALUMUX( .sel(EX_reg_1[3]), .a(rfile_rd2_out),.b(extend_immed_out), .y(alu_b) );
	// y = ( sel == 0 ) ? a : b
	alu_ctl select( EX_reg_1[1:0], funct_out, Operation );
	TotalALU alu( clk, rfile_rd1_out, alu_b, shamt_out, Operation, alu_out, rst );
	
	ifBranch br( opcode_out, Zero, rfile_rd1_out, alu_b );
	
	// RTRD
	mux2 #(5) RFMUX( .sel(EX_reg_1[2]), .a(rt_out), .b(rd_out), .y(rfile_wn) );
	
    // Stage4
	
    and BR_AND(PCSrc, MEM_reg_2[2], Zero_out);
	
	// Data Memory
	memory DatMem( .clk(clk), .MemRead(MEM_reg_2[0]), .MemWrite(MEM_reg_2[1]), .wd(rd2ToWD), .addr(aluToADDR), .rd(dmem_rdata) );	
	
	// Stage5			   
				   
	mux2 #(32) WRMUX( .sel(WB_reg_3[1]), .a(dmem_rdata_out), .b(ADDR_out), .y(rfile_wd) );			
	// y = ( sel == 0 ) ? a : b
				
	// piprline reg			   
	ifid IF_ID_Reg( .clk(clk), .rst(rst), .en_reg(1'b1), .pc_out(pc_incr_in), .ins_out(instr_out), .pc_in(pc_incr), .ins_in(instr) );
	
	idex ID_EX_Reg( .clk(clk), .rst(rst), .en_reg(1'b1), 
					.WB_out(WB_reg_1), .MEM_out(MEM_reg_1), .EX_out(EX_reg_1), .shamt_out(shamt_out), .funct_out(funct_out), .PC_out(pc_incr_out), 
					.RD1_out(rfile_rd1_out), .RD2_out(rfile_rd2_out), .immed_out(extend_immed_out), .rt_out(rt_out), .rd_out(rd_out), .op_out(opcode_out),
					.WB_in(WB_reg), .MEM_in(MEM_reg), .EX_in(EX_reg), .shamt_in(shamt), .funct_in(funct), .PC_in(pc_incr_in), 
					.RD1_in(rfile_rd1), .RD2_in(rfile_rd2), .immed_in(immed_result), .rt_in(rt), .rd_in(rd), .op_in(opcode) ) ;
					 
	exmem EX_MEM_Reg( .clk(clk), .rst(rst), .en_reg(1'b1), 
					  .WB_out(WB_reg_2), .MEM_out(MEM_reg_2), .add_out(b_out), .alu_out(aluToADDR), .RD2_out(rd2ToWD), .WN_out(rfile_wn_out), .z_in(Zero),
					  .WB_in(WB_reg_1), .MEM_in(MEM_reg_1), .add_in(b_tgt), .alu_in(alu_out), .RD2_in(rfile_rd2_out), .WN_in(rfile_wn), .z_out(Zero_out) );
	
	memwb MEM_WB_Reg( .clk(clk), .rst(rst), .en_reg(1'b1), 
					  .WB_out(WB_reg_3), .RD_out(dmem_rdata_out), .ADDR_out(ADDR_out), .WN_out(wn_2),
					  .WB_in(WB_reg_2), .RD_in(dmem_rdata), .ADDR_in(aluToADDR), .WN_in(rfile_wn_out) );


	
				   
endmodule

