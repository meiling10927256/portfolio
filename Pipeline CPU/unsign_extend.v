/*
	Title: UNSign Extend
	Author: Selene (Computer System and Architecture Lab, ICE, CYCU)
	
	Input Port
		1. immed_in: 讀入欲做sign extend資料
	Output Port
		1. ext_immed_out: 輸出已完成sign extend資料
*/
module unsign_extend( immed_in, ext_immed_out ); // 無號數擴充
	input[15:0] immed_in;
	output[31:0] ext_immed_out;
	assign ext_immed_out = { 16'd0, immed_in };
endmodule
