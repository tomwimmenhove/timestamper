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
			mrst_n_in : in  std_logic;
         capture_enable_in : in  std_logic;
         clk_in : in  std_logic;
         rst_in : in  std_logic;
         capt_in : in  std_logic;
         rst_capt_in : in  std_logic;
			count_out: out std_logic_vector(width - 1 downto 0);
         capt_count_out : out  std_logic_vector(width - 1 downto 0);
         int_out : out  std_logic
        );
    end component;

   --inputs
	signal mrst_n_in : std_logic := '0';
   signal capture_enable_in: std_logic := '0';
   signal clk_in : std_logic := '0';
   signal rst_in : std_logic := '0';
   signal capt_in : std_logic := '0';
   signal rst_capt_in : std_logic := '0';

 	--outputs
   signal capt_count_out : std_logic_vector(width- 1 downto 0);
	signal count_out : std_logic_vector(width- 1 downto 0);
   signal int_out : std_logic;

   -- clock period definitions
   constant clk_period : time := 10 ns;
 
begin
 
	-- instantiate the unit under test (uut)
   uut: capture_counter port map (
		mrst_n_in => mrst_n_in,
		capture_enable_in => capture_enable_in,
		clk_in => clk_in,
		rst_in => rst_in,
		capt_in => capt_in,
		rst_capt_in => rst_capt_in,
		count_out => count_out,
		capt_count_out => capt_count_out,
		int_out => int_out
	);

   -- clock process definitions
   clk_process :process
   begin
		clk_in <= '0';
		wait for clk_period/2;
		clk_in <= '1';
		wait for clk_period/2;
   end process;
 

   -- stimulus process
   stim_proc: process
   begin
		mrst_n_in <= '0';
		wait for clk_period * 10;
		mrst_n_in <= '1';
		wait for clk_period * 1;
		
		capture_enable_in<= '1';
		
		wait for clk_period;

		capt_in <= '1';
		wait for clk_period * 3;
		capt_in <= '0';
		
		wait for clk_period;
		
		rst_capt_in <= '1';
		wait for clk_period * 3;
		rst_capt_in <= '0';

		wait for clk_period * 14;
		capt_in <= '1';
		wait for clk_period;
		capt_in <= '0';

		capt_in <= '1';
		wait for clk_period;
		capt_in <= '0';
		
		rst_in <= '1';
		wait for clk_period * 3;
		rst_in <= '0';
		
		
      wait;
   end process;

end;
