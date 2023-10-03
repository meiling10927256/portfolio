`timescale 1ns/1ns
module Divider( clk, dataA, dataB, Signal, dataOut, reset );
input clk ;
input reset ;
input [31:0] dataA ;    // divided 
input [31:0] dataB ;    // divr
input [5:0] Signal ;
output reg [63:0] dataOut ; // rem 

//   Signal ( 6-bits)?
//   DIVU  : 27

reg [63:0] temp ;
parameter DIVU = 6'b011011 ;
reg [5:0] count ;
reg [31:0] databtemp ;
reg [31:0] rem, quot ;
	
always@( posedge clk or reset ) begin
 
    if ( Signal == 6'b011011 ) begin
		count = 6'd0 ;
		temp = { 32'd0, dataA } ;
		temp = temp << 1 ;
		databtemp = dataB ;
	end

	if ( count < 6'd32 ) begin
		if ( temp[63:32] >= databtemp ) begin
			temp[63:32] = temp[63:32] - databtemp ; 
			temp = temp << 1 ;
			temp[0] = 1'b1 ;
		end		
	   
		else begin
			temp = temp << 1 ;
			temp[0] = 1'b0 ;
		end
	   
    end 
   
    else if ( count == 6'd32 ) begin
		temp[63:32] = temp[63:32] >> 1 ;
		dataOut = temp ;
    end
	
	rem = temp[63:32];
	quot = temp[31:0] ;
	count = count + 6'd1 ;	
end


endmodule