module mux2to1( a, b, s, shift_mag ) ;

input a, b, shift_mag ; // a:別人的 b:自己的
output s ;
 
  assign s = ( shift_mag == 0 ) ? b : a ;
  
endmodule