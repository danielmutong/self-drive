// By FPGA4student.com
/******************************************************************************/
/******************  Module for reading and processing image     **************/
/******************************************************************************/
module image_read
  #(
    parameter WIDTH 	= 768, 					// Image width
    HEIGHT 	= 512, 						// Image height
    INFILE  = "kodim23.hex", 	// image file
    START_UP_DELAY = 100, 				// Delay during start up time
    VALUE = 0,
    HSYNC_DELAY = 160					// Delay between HSYNC pulses	

    )
   (
    input 	     HCLK, // clock					
    input 	     HRESETn, // Reset (active low)
    output 	     VSYNC, // Vertical synchronous pulse
    // This signal is often a way to indicate that one entire image is transmitted.
    // Just create and is not used, will be used once a video or many images are transmitted.
    output reg 	     HSYNC = 1'b0, // Horizontal synchronous pulse
    // An HSYNC indicates that one line of the image is transmitted.
    // Used to be a horizontal synchronous signals for writing bmp file.
    output reg [7:0] DATA_R0, // 8 bit Red data (even)
    output reg [7:0] DATA_G0, // 8 bit Green data (even)
    output reg [7:0] DATA_B0, // 8 bit Blue data (even)
    output reg [7:0] DATA_R1, // 8 bit Red  data (odd)
    output reg [7:0] DATA_G1, // 8 bit Green data (odd)
    output reg [7:0] DATA_B1, // 8 bit Blue data (odd)
    // Process and transmit 2 pixels in parallel to make the process faster, you can modify to transmit 1 pixels or more if needed
    output 	     ctrl_done					// Done flag
    );			
   //-------------------------------------------------
   // Internal Signals
   //-------------------------------------------------

   parameter sizeOfWidth = 8;						// data width
   parameter sizeOfLengthReal = 1179648; 		// image data : 1179648 bytes: 512 * 768 *3 
   // local parameters for FSM
   localparam		ST_IDLE 	= 2'b00,		// idle state
     ST_VSYNC	= 2'b01,			// state for creating vsync 
     ST_HSYNC	= 2'b10,			// state for creating hsync 
     ST_DATA		= 2'b11;		// state for data processing 
   reg [1:0] 	     cstate, 						// current state
		     nstate;							// next state			
   reg 		     start;									// start signal: trigger Finite state machine beginning to operate
   reg 		     HRESETn_d;								// delayed reset signal: use to create start signal
   reg 		     ctrl_vsync_run; 				// control signal for vsync counter  
   reg [8:0] 	     ctrl_vsync_cnt;			// counter for vsync
   reg 		     ctrl_hsync_run;				// control signal for hsync counter
   reg [8:0] 	     ctrl_hsync_cnt;			// counter  for hsync
   reg 		     ctrl_data_run;					// control signal for data processing
   reg [31 : 0]      in_memory    [0 : sizeOfLengthReal/4]; 	// memory to store  32-bit data image
   reg [7 : 0] 	     total_memory [0 : sizeOfLengthReal-1];	// memory to store  8-bit data image
   // temporary memory to save image data : size will be WIDTH*HEIGHT*3
   reg [7 : 0] 	     temp_BMP   [0 : WIDTH*HEIGHT*3 - 1];			
   reg [7 : 0] 	     org_R  [0 : WIDTH*HEIGHT - 1]; 	// temporary storage for R component
   reg [7 : 0] 	     org_G  [0 : WIDTH*HEIGHT - 1];	// temporary storage for G component
   reg [7 : 0] 	     org_B  [0 : WIDTH*HEIGHT - 1];	// temporary storage for B component
   // counting variables
   integer 	     i, j;
   reg [ 9:0] 	     row; // row index of the image
   reg [10:0] 	     col; // column index of the image
   reg [18:0] 	     data_count; // data counting for entire pixels of the image
   //-------------------------------------------------//
   // -------- Reading data from input file ----------//
   //-------------------------------------------------//
   initial begin
      $readmemh(INFILE,total_memory,0,sizeOfLengthReal-1); // read file from INFILE
   end
   // use 3 intermediate signals RGB to save image data
   always@(start) begin
      if(start == 1'b1) begin
         for(i=0; i<WIDTH*HEIGHT*3 ; i=i+1) begin
            temp_BMP[i] = total_memory[i+0][7:0]; 
         end
         
         for(i=0; i<HEIGHT; i=i+1) begin
            for(j=0; j<WIDTH; j=j+1) begin
               org_R[WIDTH*i+j] = temp_BMP[WIDTH*3*(HEIGHT-i-1)+3*j+0]; // save Red component
               org_G[WIDTH*i+j] = temp_BMP[WIDTH*3*(HEIGHT-i-1)+3*j+1];// save Green component
               org_B[WIDTH*i+j] = temp_BMP[WIDTH*3*(HEIGHT-i-1)+3*j+2];// save Blue component
            end
         end
      end
   end
   //----------------------------------------------------//
   // ---Begin to read image file once reset was high ---//
   // ---by creating a starting pulse (start)------------//
   //----------------------------------------------------//
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(!HRESETn) begin
           start <= 0;
	   HRESETn_d <= 0;
	end
	else begin											//        		______ 				
           HRESETn_d <= HRESETn;							//       	|		|
	   if(HRESETn == 1'b1 && HRESETn_d == 1'b0)		// __0___|	1	|___0____	: starting pulse
	     start <= 1'b1;
	   else
	     start <= 1'b0;
	end
     end

   //-----------------------------------------------------------------------------------------------//
	     // Finite state machine for reading RGB888 data from memory and creating hsync and vsync pulses --//
	     //-----------------------------------------------------------------------------------------------//
	     always@(posedge HCLK, negedge HRESETn)
	       begin
		  if(~HRESETn) begin
		     cstate <= ST_IDLE;
		  end
		  else begin
		     cstate <= nstate; // update next state 
		  end
	       end
   //-----------------------------------------//
   //--------- State Transition --------------//
   //-----------------------------------------//
   // IDLE . VSYNC . HSYNC . DATA
   always @(*) begin
      case(cstate)
	ST_IDLE: begin
	   if(start)
	     nstate = ST_VSYNC;
	   else
	     nstate = ST_IDLE;
	end			
	ST_VSYNC: begin
	   if(ctrl_vsync_cnt == START_UP_DELAY) 
	     nstate = ST_HSYNC;
	   else
	     nstate = ST_VSYNC;
	end
	ST_HSYNC: begin
	   if(ctrl_hsync_cnt == HSYNC_DELAY) 
	     nstate = ST_DATA;
	   else
	     nstate = ST_HSYNC;
	end		
	ST_DATA: begin
	   if(ctrl_done)
	     nstate = ST_IDLE;
	   else begin
	      if(col == WIDTH - 2)
		nstate = ST_HSYNC;
	      else
		nstate = ST_DATA;
	   end
	end
      endcase
   end
   // ------------------------------------------------------------------- //
   // --- counting for time period of vsync, hsync, data processing ----  //
   // ------------------------------------------------------------------- //
   always @(*) begin
      ctrl_vsync_run = 0;
      ctrl_hsync_run = 0;
      ctrl_data_run  = 0;
      case(cstate)
	ST_VSYNC: 	begin ctrl_vsync_run = 1; end 	// trigger counting for vsync
	ST_HSYNC: 	begin ctrl_hsync_run = 1; end	// trigger counting for hsync
	ST_DATA: 	begin ctrl_data_run  = 1; end	// trigger counting for data processing
      endcase
   end
   // counters for vsync, hsync
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(~HRESETn) begin
           ctrl_vsync_cnt <= 0;
	   ctrl_hsync_cnt <= 0;
	end
	else begin
           if(ctrl_vsync_run)
	     ctrl_vsync_cnt <= ctrl_vsync_cnt + 1; // counting for vsync
	   else 
	     ctrl_vsync_cnt <= 0;
	   
           if(ctrl_hsync_run)
	     ctrl_hsync_cnt <= ctrl_hsync_cnt + 1;	// counting for hsync		
	   else
	     ctrl_hsync_cnt <= 0;
	end
     end
   // counting column and row index  for reading memory 
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(~HRESETn) begin
           row <= 0;
	   col <= 0;
	end
	else begin
	   if(ctrl_data_run) begin
	      if(col == WIDTH - 2) begin
		 row <= row + 1;
	      end
	      if(col == WIDTH - 2) 
		col <= 0;
	      else 
		col <= col + 2; // reading 2 pixels in parallel
	   end
	end
     end
   //-------------------------------------------------//
   //----------------Data counting---------- ---------//
   //-------------------------------------------------//
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(~HRESETn) begin
           data_count <= 0;
	end
	else begin
           if(ctrl_data_run)
	     data_count <= data_count + 1;
	end
     end
   assign VSYNC = ctrl_vsync_run;
   assign ctrl_done = (data_count == 196607)? 1'b1: 1'b0; // done flag

   always @(posedge HCLK) begin
	
	HSYNC   <= 1'b0;
	DATA_R0 <= 0;
	DATA_G0 <= 0;
	DATA_B0 <= 0;                                       
	DATA_R1 <= 0;
	DATA_G1 <= 0;
	DATA_B1 <= 0;                                         
	if(ctrl_data_run) begin
	   HSYNC   <= 1'b1;
	   DATA_R0 <= org_R[WIDTH * row + col] + VALUE;
	   DATA_R1 <= org_R[WIDTH * row + col+1   ] + VALUE;	
	   DATA_G0 <= org_G[WIDTH * row + col   ] + VALUE;
	   DATA_G1 <= org_G[WIDTH * row + col+1   ] + VALUE;	
	   DATA_B0 <= org_B[WIDTH * row + col   ] + VALUE;
	   DATA_B1 <= org_B[WIDTH * row + col+1   ] + VALUE;	
	end else
	   HSYNC   <= 1'b0;
   end	  



endmodule




module image_write
  #(parameter WIDTH 	= 768,							// Image width
    HEIGHT 	= 512,								// Image height
    INFILE  = "output.bmp",						// Output image
    BMP_HEADER_NUM = 54							// Header for bmp image
    )
   (
    input 	HCLK, // Clock	
    input 	HRESETn, // Reset active low
    input 	hsync, // Hsync pulse						
    input [7:0] DATA_WRITE_R0, // Red 8-bit data (odd)
    input [7:0] DATA_WRITE_G0, // Green 8-bit data (odd)
    input [7:0] DATA_WRITE_B0, // Blue 8-bit data (odd)
    input [7:0] DATA_WRITE_R1, // Red 8-bit data (even)
    input [7:0] DATA_WRITE_G1, // Green 8-bit data (even)
    input [7:0] DATA_WRITE_B1, // Blue 8-bit data (even)
    output reg 	Write_Done
    );	
   integer 	BMP_header [0 : BMP_HEADER_NUM - 1];		// BMP header
   reg [7:0] 	out_BMP  [0 : WIDTH*HEIGHT*3 - 1];		// Temporary memory for image
   reg [18:0] 	data_count;									// Counting data
   wire 	done;													// done flag
   // counting variables
   integer 	i;
   integer 	k, l, m;
   integer 	fd; 
   //----------------------------------------------------------//
   //-------Header data for bmp image--------------------------//
   //----------------------------------------------------------//
   // Windows BMP files begin with a 54-byte header: 
   // Check the website to see the value of this header: http://www.fastgraph.com/help/bmp_header_format.html
   initial begin
      BMP_header[ 0] = 66;BMP_header[28] =24;
      BMP_header[ 1] = 77;BMP_header[29] = 0;
      BMP_header[ 2] = 54;BMP_header[30] = 0;
      BMP_header[ 3] =  0;BMP_header[31] = 0;
      BMP_header[ 4] = 18;BMP_header[32] = 0;
      BMP_header[ 5] =  0;BMP_header[33] = 0;
      BMP_header[ 6] =  0;BMP_header[34] = 0;
      BMP_header[ 7] =  0;BMP_header[35] = 0;
      BMP_header[ 8] =  0;BMP_header[36] = 0;
      BMP_header[ 9] =  0;BMP_header[37] = 0;
      BMP_header[10] = 54;BMP_header[38] = 0;
      BMP_header[11] =  0;BMP_header[39] = 0;
      BMP_header[12] =  0;BMP_header[40] = 0;
      BMP_header[13] =  0;BMP_header[41] = 0;
      BMP_header[14] = 40;BMP_header[42] = 0;
      BMP_header[15] =  0;BMP_header[43] = 0;
      BMP_header[16] =  0;BMP_header[44] = 0;
      BMP_header[17] =  0;BMP_header[45] = 0;
      BMP_header[18] =  0;BMP_header[46] = 0;
      BMP_header[19] =  3;BMP_header[47] = 0;
      BMP_header[20] =  0;BMP_header[48] = 0;
      BMP_header[21] =  0;BMP_header[49] = 0;
      BMP_header[22] =  0;BMP_header[50] = 0;
      BMP_header[23] =  2;BMP_header[51] = 0;	
      BMP_header[24] =  0;BMP_header[52] = 0;
      BMP_header[25] =  0;BMP_header[53] = 0;
      BMP_header[26] =  1;
      BMP_header[27] =  0;
   end
   // row and column counting for temporary memory of image 
   always@(posedge HCLK, negedge HRESETn) begin
      if(!HRESETn) begin
         l <= 0;
         m <= 0;
      end else begin
         if(hsync) begin
            if(m == WIDTH/2-1) begin
               m <= 0;
               l <= l + 1; // count to obtain row index of the out_BMP temporary memory to save image data
            end else begin
               m <= m + 1; // count to obtain column index of the out_BMP temporary memory to save image data
            end
         end
      end
   end
   // Writing RGB888 even and odd data to the temp memory
   always@(posedge HCLK, negedge HRESETn) begin
      if(!HRESETn) begin
         for(k=0;k<WIDTH*HEIGHT*3;k=k+1) begin
            out_BMP[k] <= 0;
         end
      end else begin
         if(hsync) begin
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m+2] <= DATA_WRITE_R0;
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m+1] <= DATA_WRITE_G0;
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m  ] <= DATA_WRITE_B0;
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m+5] <= DATA_WRITE_R1;
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m+4] <= DATA_WRITE_G1;
            out_BMP[WIDTH*3*(HEIGHT-l-1)+6*m+3] <= DATA_WRITE_B1;
         end
      end
   end
   // data counting
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(~HRESETn) begin
           data_count <= 0;
	end
	else begin
           if(hsync)
	     data_count <= data_count + 1; // pixels counting for create done flag
	end
     end
   assign done = (data_count == 196607)? 1'b1: 1'b0; // done flag once all pixels were processed
   always@(posedge HCLK, negedge HRESETn)
     begin
	if(~HRESETn) begin
           Write_Done <= 0;
	end
	else begin
	   Write_Done <= done;
	end
     end
   //---------------------------------------------------------//
   //--------------Write .bmp file		----------------------//
   //----------------------------------------------------------//
   initial begin
      fd = $fopen(INFILE, "wb+");
   end
   always@(Write_Done) begin // once the processing was done, bmp image will be created
      if(Write_Done == 1'b1) begin
         for(i=0; i<BMP_HEADER_NUM; i=i+1) begin
            $fwrite(fd, "%c", BMP_header[i][7:0]); // write the header
         end
         
         for(i=0; i<WIDTH*HEIGHT*3; i=i+6) begin
	    // write R0B0G0 and R1B1G1 (6 bytes) in a loop
            $fwrite(fd, "%c", out_BMP[i  ][7:0]);
            $fwrite(fd, "%c", out_BMP[i+1][7:0]);
            $fwrite(fd, "%c", out_BMP[i+2][7:0]);
            $fwrite(fd, "%c", out_BMP[i+3][7:0]);
            $fwrite(fd, "%c", out_BMP[i+4][7:0]);
            $fwrite(fd, "%c", out_BMP[i+5][7:0]);
         end
      end
   end
endmodule
