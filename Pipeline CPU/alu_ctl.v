/*
	Title:	ALU Control Unit
	Author: Garfield (Computer System and Architecture Lab, ICE, CYCU)
	Input Port
		1. ALUOp: 控制alu是要用+還是-或是其他指令
		2. Funct: 如果是其他指令則用這邊6碼判斷
	Output Port
		1. ALUOperation: 最後解碼完成之指令
*/

module alu_ctl(ALUOp, Funct, ALUOperation );
    input [1:0] ALUOp;
    input [5:0] Funct;
	
    output [5:0] ALUOperation;
    reg    [5:0] ALUOperation;

    // symbolic constants for instruction function code
    parameter F_add = 6'd32;
    parameter F_sub = 6'd34;
    parameter F_and = 6'd36;
    parameter F_or  = 6'd37;
    parameter F_slt = 6'd42;

    parameter F_sll = 6'd0;
	parameter F_divu = 6'd27;
    parameter F_mfhi = 6'd16;
    parameter F_mflo = 6'd18;
	
    // symbolic constants for ALU Operations
    parameter ALU_add = 6'b100000;
    parameter ALU_sub = 6'b100010;
    parameter ALU_and = 6'b100100;
    parameter ALU_or  = 6'b100101;
    parameter ALU_slt = 6'b101010;
	
	parameter ALU_sll = 6'b000000;
	parameter ALU_divu = 6'b011011; 	
	parameter ALU_mfhi = 6'b010000; 
	parameter ALU_mflo = 6'b010010; 
	
    always @(ALUOp or Funct)
    begin

        case (ALUOp) 
            2'b00 : ALUOperation = ALU_add;
            2'b01 : ALUOperation = ALU_sub;
			2'b11 : ALUOperation = ALU_or;
            2'b10 : case (Funct) 
                        F_add : ALUOperation = ALU_add;
                        F_sub : ALUOperation = ALU_sub;
                        F_and : ALUOperation = ALU_and;
                        F_or  : ALUOperation = ALU_or;
                        F_slt : ALUOperation = ALU_slt;
						F_sll : ALUOperation = ALU_sll;
						F_divu : ALUOperation = ALU_divu;
						F_mfhi : ALUOperation = ALU_mfhi ;
						F_mflo : ALUOperation = ALU_mflo ; 
                        default ALUOperation = 6'bxxxxxx;
                    endcase
            default ALUOperation = 6'bxxxxxx;
        endcase
    end
endmodule

