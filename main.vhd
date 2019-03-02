----------------------------------------------------------------------------------
-- company: 
-- engineer: Tom Wimmenhove
-- 
-- create date:    14:48:25 02/24/2019 
-- design name: 
-- module name:    main - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: 
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
entity main is
	generic (
		width: integer := 27 -- fits 100mhz*1s
	);
	 
	port (
		-- master reset
		mrst_n : in  std_logic;
		
		-- counter
		clk: in std_logic;
		pps: in std_logic;
		
		-- counter capture
		capt: in std_logic;
		rstcapt: in std_logic;
			
		-- serial output
		sclk: in std_logic;
		ce_n: in std_logic;
		sdo: out std_logic;
		
		-- '1' when there's captured data
		int: out std_logic
	);
end main;

architecture behavioral of main is
	signal count_latch: std_logic_vector(width - 1 downto 0);	-- the counter (capture) latch
	signal count_byte_latch: std_logic_vector((width + (((8 - (width mod 8)) mod 8))) - 1 downto 0);	-- left-justified byte-aligned representation of the counter latch
	
	signal spi_sdo: std_logic;
begin
	-- counter instance
	i_capture_counter_ts: entity work.capture_counter
	generic map(
		width => width
	)
	port map(
		mrst_n => mrst_n,
		capture_enable => ce_n, -- don't capture duing an spi transaction.
		clk => clk,
		rst => pps,					-- reset on pps pulse
		capt => capt,
		rstcapt => rstcapt,
		latch => count_latch,
		int => int
	);
	
	-- align the counter value
	count_byte_latch <= std_logic_vector(resize(unsigned(count_latch), count_byte_latch'length));

	-- only drive the sdo pin (with spi_sdo) when chip_select is assreted (low)
	with ce_n select sdo <= spi_sdo when '0', 'Z' when others;

	-- spi instance
	i_spi_out: entity work.serial_out
	generic map(
		txwidth => count_byte_latch'length
	)
	port map(
		txreg => count_byte_latch,
		sclk => sclk,
		ce_n => ce_n,
		sdo => spi_sdo
	);
end behavioral;

