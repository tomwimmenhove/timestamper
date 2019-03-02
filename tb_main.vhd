--------------------------------------------------------------------------------
-- company: 
-- engineer:
--
-- create date:   20:42:24 03/01/2019
-- design name:   
-- module name:   /home/tom/grive-tom.wimmenhove/projects/cosmic/hardware/capture_counter/tb_main.vhd
-- project name:  capture_counter
-- target device:  
-- tool versions:  
-- description:   
-- 
-- vhdl test bench created by ise for module: main
-- 
-- dependencies:
-- 
-- revision:
-- revision 0.01 - file created
-- additional comments:
--
-- notes: 
-- this testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  xilinx recommends
-- that these types always be used for the top-level i/o of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
 
-- uncomment the following library declaration if using
-- arithmetic functions with signed or unsigned values
--use ieee.numeric_std.all;
 
entity tb_main is
	generic (
		width: integer := 4
	);
end tb_main;
 
architecture behavior of tb_main is 
 
    -- component declaration for the unit under test (uut)
 
    component main
	 generic (
		width: integer := width
	 );
    port(
         mrst_n : in  std_logic;
         clk : in  std_logic;
         pps : in  std_logic;
         capt : in  std_logic;
         rstcapt : in  std_logic;
         sclk : in  std_logic;
         ce_n : in  std_logic;
         sdo : out  std_logic;
         int : out  std_logic
        );
    end component;
    

   --inputs
   signal mrst_n : std_logic := '0';
   signal clk : std_logic := '0';
   signal pps : std_logic := '0';
   signal capt : std_logic := '0';
   signal rstcapt : std_logic := '0';
   signal sclk : std_logic := '0';
   signal ce_n : std_logic := '0';

 	--outputs
   signal sdo : std_logic;
   signal int : std_logic;

   -- clock period definitions
   constant clk_period : time := 10 ns;
 
begin
 
	-- instantiate the unit under test (uut)
   uut: main port map (
          mrst_n => mrst_n,
          clk => clk,
          pps => pps,
          capt => capt,
          rstcapt => rstcapt,
          sclk => sclk,
          ce_n => ce_n,
          sdo => sdo,
          int => int
        );

   -- clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;

   -- stimulus process
   stim_proc: process
   begin		
		mrst_n <= '0';
		ce_n <= '1';
      wait for clk_period * 10;
		mrst_n <= '1';
		
		wait for clk_period * 6;
		
		capt <= '1';
		wait for clk_period;
		capt <= '0';
		
		wait for clk_period;

		rstcapt <= '1';
		wait for clk_period;
		rstcapt <= '0';
		wait for clk_period ;
	
		pps <= '1';
		wait for clk_period * 2;
		pps <= '0';
		
		wait for clk_period * 6;
		
		capt <= '1';
		wait for clk_period;
		capt <= '0';
		
		ce_n <= '0';
		for i in 0 to 7 loop
			wait for clk_period / 2;
			sclk <= '1';
			wait for clk_period / 2;
			sclk <= '0';
		end loop;
		wait for clk_period;
		ce_n <= '1';
		
      wait;
   end process;

end;
