`timescale 1ns/1ps

`include "writescalar.v"
`include "readscalar.v"
`include "scalar.v"
`define INPUTFILENAME "kodim23.hex"
`define OUTPUTFILENAME "output.bmp"
`define SIZE 518400
`define H_pixels 1920
`define V_pixels 1080
`define BUS_WIDTH ($clog2(`SIZE) - 1)

module tb_simulation;

   reg HCLK, HRESETn;
   wire vsync;
   wire hsync;
   wire [ 7 : 0] data_R;
   wire [ 7 : 0] data_G;
   wire [ 7 : 0] data_B;
   wire          enc_done;

   wire          enable;
   wire          clk;
   wire          H_sig;
   wire          V_sig;
   wire          de;
   
   wire [7:0]    dout;
   wire          write_signal;
   wire [`BUS_WIDTH:0] wa;
   wire [7:0]          gray;
   wire [7:0]          scale_out;
   
   wire [7:0]          ram_out;
   
   
   
   

   initial begin
      $dumpfile("test.vcd");
      $dumpvars;
      #10000000;
      $finish;
      
   end

 
   image_read 
     #(.INFILE(`INPUTFILENAME))
   u_image_read
     ( 
       .HCLK	                (HCLK    ),
       .HRESETn	            (HRESETn ),
       .VSYNC	                (vsync   ),
       .HSYNC	                (hsync   ),
       .DE(de),
       .DATA_R	            (data_R ),
       .DATA_G	            (data_G ),
       .DATA_B	            (data_B ),

       .gray (gray),
       .ctrl_done				(enc_done)
       ); 

   scalar SCALAR(.din(gray), .enable(de), .clk(HCLK), .H_sig(hsync), .V_sig(vsync), .dout(scale_out), .write_signal(write_signal), .wa(wa), .ram_out(ram_out));
   
   
   image_write 
     #(.INFILE(`OUTPUTFILENAME))
   u_image_write
     (
      .HCLK(HCLK),
      .HRESETn(HRESETn),
      .hsync(hsync),
      .DATA_WRITE_R(data_R),
      .DATA_WRITE_G(data_G),
      .DATA_WRITE_B(data_B),
 
      .Write_Done()
      );	

   initial begin 
      HCLK = 0;
      forever #10 HCLK = ~HCLK;
   end

   initial begin
      HRESETn     = 0;
      #25 HRESETn = 1;
   end


endmodule

