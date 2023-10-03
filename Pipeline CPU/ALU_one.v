module ALU_one( cout, sum, a, b, sel, invert, cin, less ) ;

  input a, b, invert, cin, less ;
  input wire[1:0] sel ;
  output cout, sum ; 
  wire in0, in1, xor1, in2, in3, in4 ; 
  
  assign in0 = a & b ; // AND
  assign in1 = a | b ; // OR
  assign xor1 = invert ^ b ; // ADD || SUB
  FA fa( .sum(in2), .cout(cout), .a(a), .b(xor1), .c(cin) ) ; // FA 
  
  assign sum = ( sel == 2'b00 ) ? in0 : ( sel == 2'b01 ) ? in1 :( sel == 2'b10 ) ? in2 : less ; 

endmodule
