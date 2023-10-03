module mux2( sel, a, b, y );
parameter bitwidth = 32; 
input sel ;
input [bitwidth-1 : 0] a, b ; // a:別人的 b:自己的
output [bitwidth-1 : 0] y ;
 
  assign y = ( sel ) ? b : a ;
  
endmodule