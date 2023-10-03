`timescale 1ns/1ns
module ALU( dataA, dataB, Signal, dataOut, reset );
input reset ;
input [31:0] dataA ;
input [31:0] dataB ;
input [5:0] Signal ;
output [31:0] dataOut ;

wire [31:0] temp ;

parameter AND = 6'b100100; // 36
parameter OR  = 6'b100101; // 37
parameter ADD = 6'b100000; // 32
parameter SUB = 6'b100010; // 34
parameter SLT = 6'b101010; // 42

wire[1:0] sel;
wire invert, less ;
wire[32:0] dataC ; 

  assign sel    = ( Signal == AND ) ? 2'b00 : ( Signal == OR ) ? 2'b01 : ( Signal == ADD ) ? 2'b10 : ( Signal == SUB ) ? 2'b10 : 2'b11 ;
  assign invert = ( Signal == AND ) ? 1'b0  : ( Signal == OR ) ? 1'b0  : ( Signal == ADD ) ? 1'b0  : ( Signal == SUB ) ? 1'b1  : 1'b1 ;  
  
  //0.
  assign dataC[0] = invert ;
  assign less = ( Signal == SLT && dataA < dataB ) ? 1'b1 : 1'b0 ;
  
  ALU_one alu_one0( .cout(dataC[1]), .sum(temp[0]), .a(dataA[0]), .b(dataB[0]), .sel(sel), .invert(invert), .cin(dataC[0]), .less(less) ) ;

  //1. 
  ALU_one alu_one1( .cout(dataC[2]), .sum(temp[1]), .a(dataA[1]), .b(dataB[1]), .sel(sel), .invert(invert), .cin(dataC[1]), .less(1'b0) ) ;

  //2. 
  ALU_one alu_one2( .cout(dataC[3]), .sum(temp[2]), .a(dataA[2]), .b(dataB[2]), .sel(sel), .invert(invert), .cin(dataC[2]), .less(1'b0) ) ;

  //3. 
  ALU_one alu_one3( .cout(dataC[4]), .sum(temp[3]), .a(dataA[3]), .b(dataB[3]), .sel(sel), .invert(invert), .cin(dataC[3]), .less(1'b0) ) ;

  //4. 
  ALU_one alu_one4( .cout(dataC[5]), .sum(temp[4]), .a(dataA[4]), .b(dataB[4]), .sel(sel), .invert(invert), .cin(dataC[4]), .less(1'b0) ) ;

  //5. 
  ALU_one alu_one5( .cout(dataC[6]), .sum(temp[5]), .a(dataA[5]), .b(dataB[5]), .sel(sel), .invert(invert), .cin(dataC[5]), .less(1'b0) ) ;

  //6.
  ALU_one alu_one6( .cout(dataC[7]), .sum(temp[6]), .a(dataA[6]), .b(dataB[6]), .sel(sel), .invert(invert), .cin(dataC[6]), .less(1'b0) ) ;

  //7. 
  ALU_one alu_one7( .cout(dataC[8]), .sum(temp[7]), .a(dataA[7]), .b(dataB[7]), .sel(sel), .invert(invert), .cin(dataC[7]), .less(1'b0) ) ;

  //8. 
  ALU_one alu_one8( .cout(dataC[9]), .sum(temp[8]), .a(dataA[8]), .b(dataB[8]), .sel(sel), .invert(invert), .cin(dataC[8]), .less(1'b0) ) ;

  //9. 
  ALU_one alu_one9( .cout(dataC[10]), .sum(temp[9]), .a(dataA[9]), .b(dataB[9]), .sel(sel), .invert(invert), .cin(dataC[9]), .less(1'b0) ) ;

  //10. 
  ALU_one alu_one10( .cout(dataC[11]), .sum(temp[10]), .a(dataA[10]), .b(dataB[10]), .sel(sel), .invert(invert), .cin(dataC[10]), .less(1'b0) ) ;

  //11. 
  ALU_one alu_one11( .cout(dataC[12]), .sum(temp[11]), .a(dataA[11]), .b(dataB[11]), .sel(sel), .invert(invert), .cin(dataC[11]), .less(1'b0) ) ;

  //12. 
  ALU_one alu_one12( .cout(dataC[13]), .sum(temp[12]), .a(dataA[12]), .b(dataB[12]), .sel(sel), .invert(invert), .cin(dataC[12]), .less(1'b0) ) ;
  
  //13. 
  ALU_one alu_one13( .cout(dataC[14]), .sum(temp[13]), .a(dataA[13]), .b(dataB[13]), .sel(sel), .invert(invert), .cin(dataC[13]), .less(1'b0) ) ;

  //14. 
  ALU_one alu_one14( .cout(dataC[15]), .sum(temp[14]), .a(dataA[14]), .b(dataB[14]), .sel(sel), .invert(invert), .cin(dataC[14]), .less(1'b0) ) ;

  //15. 
  ALU_one alu_one15( .cout(dataC[16]), .sum(temp[15]), .a(dataA[15]), .b(dataB[15]), .sel(sel), .invert(invert), .cin(dataC[15]), .less(1'b0) ) ;

  //16. 
  ALU_one alu_one16( .cout(dataC[17]), .sum(temp[16]), .a(dataA[16]), .b(dataB[16]), .sel(sel), .invert(invert), .cin(dataC[16]), .less(1'b0) ) ;

  //17. 
  ALU_one alu_one17( .cout(dataC[18]), .sum(temp[17]), .a(dataA[17]), .b(dataB[17]), .sel(sel), .invert(invert), .cin(dataC[17]), .less(1'b0) ) ;
 
  //18. 
  ALU_one alu_one18( .cout(dataC[19]), .sum(temp[18]), .a(dataA[18]), .b(dataB[18]), .sel(sel), .invert(invert), .cin(dataC[18]), .less(1'b0) ) ;

  //19. 
  ALU_one alu_one19( .cout(dataC[20]), .sum(temp[19]), .a(dataA[19]), .b(dataB[19]), .sel(sel), .invert(invert), .cin(dataC[18]), .less(1'b0) ) ;
  
  //20. 
  ALU_one alu_one20( .cout(dataC[21]), .sum(temp[20]), .a(dataA[20]), .b(dataB[20]), .sel(sel), .invert(invert), .cin(dataC[20]), .less(1'b0) ) ;

  //21. 
  ALU_one alu_one21( .cout(dataC[22]), .sum(temp[21]), .a(dataA[21]), .b(dataB[21]), .sel(sel), .invert(invert), .cin(dataC[21]), .less(1'b0) ) ;

  //22. 
  ALU_one alu_one22( .cout(dataC[23]), .sum(temp[22]), .a(dataA[22]), .b(dataB[22]), .sel(sel), .invert(invert), .cin(dataC[22]), .less(1'b0) ) ;

  //23. 
  ALU_one alu_one23( .cout(dataC[24]), .sum(temp[23]), .a(dataA[23]), .b(dataB[23]), .sel(sel), .invert(invert), .cin(dataC[23]), .less(1'b0) ) ;

  //24. 
  ALU_one alu_one24( .cout(dataC[25]), .sum(temp[24]), .a(dataA[24]), .b(dataB[24]), .sel(sel), .invert(invert), .cin(dataC[24]), .less(1'b0) ) ;

  //25. 
  ALU_one alu_one25( .cout(dataC[26]), .sum(temp[25]), .a(dataA[25]), .b(dataB[25]), .sel(sel), .invert(invert), .cin(dataC[25]), .less(1'b0) ) ;
  
  //26. 
  ALU_one alu_one26( .cout(dataC[27]), .sum(temp[26]), .a(dataA[26]), .b(dataB[26]), .sel(sel), .invert(invert), .cin(dataC[26]), .less(1'b0) ) ;

  //27. 
  ALU_one alu_one27( .cout(dataC[28]), .sum(temp[27]), .a(dataA[27]), .b(dataB[27]), .sel(sel), .invert(invert), .cin(dataC[27]), .less(1'b0) ) ;
  
  //28. 
  ALU_one alu_one28( .cout(dataC[29]), .sum(temp[28]), .a(dataA[28]), .b(dataB[28]), .sel(sel), .invert(invert), .cin(dataC[28]), .less(1'b0) ) ;
  
  //29. 
  ALU_one alu_one29( .cout(dataC[30]), .sum(temp[29]), .a(dataA[29]), .b(dataB[29]), .sel(sel), .invert(invert), .cin(dataC[29]), .less(1'b0) ) ;
  
  //30. 
  ALU_one alu_one30( .cout(dataC[31]), .sum(temp[30]), .a(dataA[30]), .b(dataB[30]), .sel(sel), .invert(invert), .cin(dataC[30]), .less(1'b0) ) ;
  
  //31. 
  ALU_one alu_one31( .cout(dataC[32]), .sum(temp[31]), .a(dataA[31]), .b(dataB[31]), .sel(sel), .invert(invert), .cin(dataC[31]), .less(1'b0) ) ;
 
  assign dataOut = ( reset ) ? 32'd0 : temp ;
  
endmodule
