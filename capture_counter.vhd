----------------------------------------------------------------------------------
-- company: 
-- engineer: Tom Wimmenhove
-- 
-- create date:    19:00:21 02/26/2019 
-- design name:    timestamp capture counter
-- module name:    capture_counter - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: captures the time of capture pulses
-- 
--
-- dependencies: 
--
-- revision: 
-- revision 0.01 - file created
-- additional comments: 
--
----------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity capture_counter is
	generic (
		width: integer := 8
	);
	 
	port (
		mrst_n_in: in std_logic;				-- master reset
	
		capture_enable_in: in std_logic;		-- enable capturing (active high)
		
		clk_in: in std_logic;					-- 100mhz clock input
		rst_in: in std_logic;					-- synchronous reset (resets when high on first rising clock pulse)
		
		capt_in: in std_logic;					-- synchronous capture current count into latch (high while first rising clock pulse), then disables capturing
		rst_capt_in: in std_logic;				-- synchronous re-enable of capturing (high while first rising clock pulse)
		
		count_out: out std_logic_vector(width - 1 downto 0);	-- reg containing the current count
		capt_count_out: out std_logic_vector(width - 1 downto 0);	-- reg containing the captured count
		int_out: out std_logic					-- high when new data available in latch
	);
end capture_counter;

architecture behavioral of capture_counter is
	signal count: std_logic_vector(width - 1 downto 0);
	signal wait_reset: std_logic;
	signal rst_armed: std_logic;
	
begin
	count_out <= count;
	
	counterprocess: process(mrst_n_in , clk_in)
	begin
		if mrst_n_in = '0' then
			rst_armed <= '1';
			count <= (others => '0');
		else
			if rising_edge(clk_in) then
				if rst_armed = '1' then
					if rst_in = '1' then
						count <= (others => '0');
						rst_armed <= '0';
					else
						count <= count + 1;
						rst_armed <= '1';
					end if;
				else
					count <= count + 1;
					if rst_in = '0' then
						rst_armed <= '1';
					else
						rst_armed <= '0';
					end if;
				end if;
			end if;
		end if;
	end process;
		
	captureprocess: process(mrst_n_in, clk_in)
	begin
		if mrst_n_in = '0' then
			wait_reset <= '0';
			int_out <= '0';
			capt_count_out <= (others => '0');
		else
			if rising_edge(clk_in) then
				if rst_capt_in = '1' then
					wait_reset <= '0';
					int_out <= '0';
				else
					if capture_enable_in = '1' and wait_reset = '0' and capt_in = '1' then
						capt_count_out <= count;
						wait_reset <= '1';
						int_out <= '1';
					end if;
				end if;
			end if;
		end if;
	end process;
end behavioral;

