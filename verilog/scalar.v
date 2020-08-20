`include "ram.v"
`define WAIT_V_STATE 3'b000
`define WAIT_H_STATE 3'b001
`define WAIT_ENABLE_STATE 3'b010
`define DATA_STATE 3'b011
`define DONE_STATE 3'b100
//`define H_pixels 10'b1100000000
//`define V_pixels 10'b1000000000
`define SIZE 518400
//`define H_pixels 1920
//`define V_pixels 1080
`define H_pixels 768
`define V_pixels 512

`define BUS_WIDTH ($clog2(`SIZE) - 1)
module scalar( din, enable, clk, H_sig, V_sig,  dout, write_signal,  wa, ram_out);
   input  [7:0] din;
   input 	    enable;
   input 	    clk;
   input 	    H_sig;
   input 	    V_sig;
   output  [7:0] dout;
   output reg 	     write_signal;
   output reg [`BUS_WIDTH:0]   wa=0;
   
   
   reg [2:0] 	     c_state = 3'b0;
   reg [2:0] 	     n_state = 3'b0;
   reg [`H_pixels-1:0]      pixelcnt;
   reg [`V_pixels-1:0]      linecnt;
   reg [`BUS_WIDTH:0] 	    ra=0;
   reg [7:0] 		    scale;
   output  [7:0] 		    ram_out;
   
 //  wire 		    write;
   
   
//istantiating ram
   ram_reg RAM_BLOCK(.ra(ra), .wa(wa), .write(write_signal),.din(scale),.dout(ram_out));
   
   
   //state machine
   //transitions
   always@(posedge clk) begin;
      
      c_state=n_state;
   end
   
   always @(*) begin
      case (c_state) 
	`WAIT_V_STATE:
	  begin
	     if (V_sig)
	       n_state=`WAIT_H_STATE;
	  end
	`WAIT_H_STATE:
	  begin
	     if (H_sig && linecnt[0] == 0)
	       n_state=`WAIT_ENABLE_STATE;
	  end
	`WAIT_ENABLE_STATE:
	  begin
	     if(pixelcnt==`H_pixels) begin
		if(linecnt==`V_pixels)
		  n_state=`DONE_STATE;
		else
		  n_state=`WAIT_H_STATE;
	     end
	  end
	default:
	  n_state=c_state;
	
      endcase
   end // always @ (*)
   
   always@(posedge clk) begin
      write_signal<=0;
      if(c_state == `WAIT_ENABLE_STATE && enable) begin
	 pixelcnt <= pixelcnt+1;
	 if(pixelcnt[0] == 0)begin
	    scale <= din;
	    wa <= wa+1;
	    write_signal <= 1;
	 end
      end else if (H_sig)
	pixelcnt<=0;
   end
	 
   always@(posedge clk) begin
      if (V_sig)
	linecnt<=0;
      else if (H_sig)
	linecnt <= linecnt+1;
   end
      
   assign dout = scale;
   
endmodule
