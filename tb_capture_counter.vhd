--------------------------------------------------------------------------------
-- company: 
-- engineer:
--
-- create date:   19:33:50 03/01/2019
-- design name:   
-- module name:   /home/tom/grive-tom.wimmenhove/projects/cosmic/hardware/capture_counter/tb_capture_counter.vhd
-- project name:  capture_counter
-- target device:  
-- tool versions:  
-- description:   
-- 
-- vhdl test bench created by ise for module: capture_counter
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
 
entity tb_capture_counter is
	generic (
		width: integer := 4
	);
end tb_capture_counter;
 
architecture behavior of tb_capture_counter is 
 
    -- component declaration for the unit under test (uut)
 
    component capture_counter
	 	generic (
		width: integer := width
	);
	 port(
			mrst_n : in  std_logic;
         capture_enable : in  std_logic;
         clk : in  std_logic;
         rst : in  std_logic;
         capt : in  std_logic;
         rstcapt : in  std_logic;
         latch : out  std_logic_vector(width - 1 downto 0);
         int : out  std_logic
        );
    end component;

   --inputs
	signal mrst_n : std_logic := '0';
   signal capture_enable : std_logic := '0';
   signal clk : std_logic := '0';
   signal rst : std_logic := '0';
   signal capt : std_logic := '0';
   signal rstcapt : std_logic := '0';

 	--outputs
   signal latch : std_logic_vector(width- 1 downto 0);
   signal int : std_logic;

   -- clock period definitions
   constant clk_period : time := 10 ns;
 
begin
 
	-- instantiate the unit under test (uut)
   uut: capture_counter port map (
		mrst_n => mrst_n,
		capture_enable => capture_enable,
		clk => clk,
		rst => rst,
		capt => capt,
		rstcapt => rstcapt,
		latch => latch,
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
		wait for clk_period * 10;
		mrst_n <= '1';
		wait for clk_period * 1;
		
		capture_enable <= '1';
		
		wait for clk_period;

		capt <= '1';
		wait for clk_period * 3;
		capt <= '0';
		
		wait for clk_period;
		
		rstcapt <= '1';
		wait for clk_period * 3;
		rstcapt <= '0';

		wait for clk_period * 14;
		capt <= '1';
		wait for clk_period;
		capt <= '0';

		capt <= '1';
		wait for clk_period;
		capt <= '0';
		
		rst <= '1';
		wait for clk_period * 3;
		rst <= '0';
		
		
      wait;
   end process;

end;
