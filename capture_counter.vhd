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
		mrst_n: in std_logic;				-- master reset
	
		capture_enable: in std_logic;		-- enable capturing (active high)
		
		clk: in std_logic;					-- 100mhz clock input
		rst: in std_logic;					-- synchronous reset (resets when high on first rising clock pulse)
		
		capt: in std_logic;					-- synchronous capture current count into latch (high while first rising clock pulse), then disables capturing
		rstcapt: in std_logic;				-- synchronous re-enable of capturing (high while first rising clock pulse)
		
		latch: out std_logic_vector(width - 1 downto 0);	-- output latch containing the captured count
		int: out std_logic					-- high when new data available in latch
	);
end capture_counter;

architecture behavioral of capture_counter is
	signal counter: std_logic_vector(width - 1 downto 0);
	signal wait_reset: std_logic;
	
	signal rst_armed: std_logic;
begin
	counterprocess: process(mrst_n , clk)
	begin
		if mrst_n = '0' then
			rst_armed <= '1';
			counter <= (others => '0');
		else
			if rising_edge(clk) then
				if rst_armed = '1' then
					if rst = '1' then
						counter <= (others => '0');
						rst_armed <= '0';
					else
						counter <= counter + 1;
						rst_armed <= '1';
					end if;
				else
					counter <= counter + 1;
					if rst = '0' then
						rst_armed <= '1';
					else
						rst_armed <= '0';
					end if;
				end if;
			end if;
		end if;
	end process;
		
	captureprocess: process(mrst_n, clk)
	begin
		if mrst_n = '0' then
			wait_reset <= '0';
			int <= '0';
			latch <= (others => '0');
		else
			if rising_edge(clk) then
				if rstcapt = '1' then
					wait_reset <= '0';
					int <= '0';
				else
					if capture_enable = '1' and wait_reset = '0' and capt = '1' then
						latch <= counter;
						wait_reset <= '1';
						int <= '1';
					end if;
				end if;
			end if;
		end if;
	end process;
end behavioral;

