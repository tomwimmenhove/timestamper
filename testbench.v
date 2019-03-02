`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   15:48:04 02/24/2019
// Design Name:   main
// Module Name:   /home/tom/grive-Tom.Wimmenhove/Projects/cpld_test/hdl_test/testbench.v
// Project Name:  hdl_test
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: main
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module testbench;

	reg [5:1] x;

	// Inputs
	reg CLK;
	reg RST;
	reg CAPT;
	reg SCLK;
	reg RSTCAPT;
	reg CE_N;

	// Outputs
	wire SDO;
	wire INT;

	// Instantiate the Unit Under Test (UUT)
	main uut (
		.CLK(CLK), 
		.RST(RST), 
		.CAPT(CAPT),
		.SDO(SDO),
		.SCLK(SCLK),
		.CE_N(CE_N),
		.RSTCAPT(RSTCAPT),
		.INT(INT)
	);

	initial begin
		// Initialize Inputs
		CLK = 0;
		RST = 0;
		SCLK = 0;
		CAPT = 0;
		RSTCAPT = 0;
		CE_N = 1;
		
		x = 5'b00000;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here
		
		// Reset the count
		RST = 1;
		#10;
		CLK = 1;
		#10;
		CLK = 0;
		#10;
		RST = 0;
		#10;

		// Enable capturing: INT should be low
		RSTCAPT  = 1;
		#10;
		RSTCAPT  = 0;
		#10;
		
		// Clock 10 pulses
		for (x=0; x < 10; x= x+1)
		begin
			CLK = 1;
			#10;
			CLK = 0;
			#10;
		end // end of for loop
		
		// Capture current count
		CAPT = 1;
		#10;
		CAPT = 0;
		#10;
		
		// SPI Chip enable
		CE_N = 0;
		#10;
	
		// Clock out first two bits
		for (x=0; x < 2; x= x+1)
		begin
			SCLK = 1;
			#10;
			SCLK = 0;
			#10;
		end;


		// Select some other chip that's NOT us.
		CE_N = 1;
		#10;
		
		// Clock some
		for (x=0; x < 2; x= x+1)
		begin
			SCLK = 1;
			#10;
			SCLK = 0;
			#10;
		end;
		
		// Select us again
		CE_N = 0;
		#10;
		
		// Clock whole byte
		for (x=0; x < 8; x= x+1)
		begin
			SCLK = 1;
			#10;
			SCLK = 0;
			#10;
		end;
		
		// Reset the count
		RST = 1;
		#10;
	
		// Count some more
		for (x=0; x < 10; x= x+1)
		begin
			CLK = 1;
			#10;
			CLK = 0;
			#10;
		end // end of for loop

		// reset pulse is looooong
		RST = 0;
		#10;


		// Capture pulse while we already have something in the capture latch. Should be ignored
		CAPT = 1;
		#10;
		CAPT = 0;
		#10;

		// Clock out the last bits
		for (x=0; x < 2; x= x+1)
		begin
			SCLK = 1;
			#10;
			SCLK = 0;
			#10;
		end;
		
		CE_N = 1;
		#10;


		// Re-enable capture
		RSTCAPT = 1;
		#10;
		RSTCAPT = 0;
		#10;

		// Count some more
		for (x=0; x < 9; x= x+1)
		begin
			CLK = 1;
			#10;
			CLK = 0;
			#10;
		end // end of for loop

		// Capture the count
		CAPT = 1;
		#10;
		CAPT = 0;
		#10;

		CE_N = 0;
		#10;

		// Clock out the captured count.
		for (x=0; x < 8; x= x+1)
		begin
			SCLK = 1;
			#10;
			SCLK = 0;
			#10;
		end // end of for loop
		
		CE_N = 1;
		#10;

	end
      
endmodule

