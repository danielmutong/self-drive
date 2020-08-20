module ram_reg_tb;
   
   wire [:0] din;
   wire [7:0] dout;
   wire [BUS_WIDTH : 0] ra, wa;
   wire 	    write;
   wire [7:0] 	    ram [SIZE-1:0];



   ram_reg test (.ra(ra), .wa(wa), .write(write), .din(din), .dout(dout));
   
   
   initial // initial block executes only once
     begin
     
     end
endmodule
