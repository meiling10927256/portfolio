module FA(a, b, c, cout, sum) ;
  input a, b, c ;
  output cout, sum ;
  
  wire e1, e2, e3 ;
  assign e1 = a & b ;
  assign e2 = a & c ;
  assign e3 = b & c ;
  assign cout = e1 | e2 | e3 ;
  assign sum = a ^ b ^ c ;
  
endmodule
