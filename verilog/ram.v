`define H_pixels 1920
`define V_pixels 1080
`define SIZE 518400
`define BUS_WIDTH ($clog2(`SIZE) - 1)
module ram_reg (ra, wa, write, din, dout);
   
   input [7 : 0] din;
   output  [7:0] dout;
   input [`BUS_WIDTH : 0] ra, wa;
   input 	    write;
   input 	    read;
   
   reg [7:0] 	    ram [`SIZE-1:0];
   assign dout = ram[ra];

   always@(*) begin
      if(write)
	ram[wa]=din;
  
      
   end
endmodule // ram_reg



 
 
