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
		--width: integer := 26 -- fits 50mhz*1s
	);
	 
	port (
		-- master reset
		mrst_n_in : in  std_logic;
		
		-- counter
		clk_in: in std_logic;
		pps_in: in std_logic;
		
		-- counter capture
		capt_in: in std_logic;
		rst_capt_in: in std_logic;
		
		-- serial output
		sclk_in: in std_logic;
		ce_n_in: in std_logic;
		sdo_out: out std_logic;
		
		-- high when there's captured data
		int_n_out: out std_logic;
		
		-- Test outputs
		led_hb_out: out std_logic; -- Top bit of the counter
		led_pps_out: out std_logic;
		led_capt_out: out std_logic
	);
end main;

architecture behavioral of main is
	signal count: std_logic_vector(width - 1 downto 0);
	signal capt_count: std_logic_vector(width - 1 downto 0);	-- the counter (capture) latch
	signal capt_count_byte_align: std_logic_vector((width + (((8 - (width mod 8)) mod 8))) - 1 downto 0);	-- left-justified byte-aligned representation of the counter latch

	signal pps_in_sync: std_logic;
	signal capt_in_sync: std_logic;
	signal rst_capt_in_sync: std_logic;
	
	signal spi_sdo: std_logic;
begin
	-- Synchronise inputs
	syncprocess: process(clk_in)
	begin
		if falling_edge(clk_in) then
			-- XXX: Don't forget to invert these for the real PCB. There are inverters at the inputs!
			pps_in_sync <= pps_in;
			capt_in_sync <= capt_in;
			rst_capt_in_sync <=rst_capt_in;
		end if;
	end process;

	-- counter instance
	i_capture_counter_ts: entity work.capture_counter
	generic map(
		width => width
	)
	port map(
		mrst_n_in => mrst_n_in,
		capture_enable_in => ce_n_in, -- don't capture duing an spi transaction.
		clk_in => clk_in,
		rst_in => pps_in_sync,					-- reset on pps pulse
		capt_in => capt_in_sync,
		rst_capt_in => rst_capt_in_sync,
		count_out => count,
		capt_count_out => capt_count,
		int_n_out => int_n_out
	);
	
	led_hb_out <= count(width - 1); -- Output the highest bit of the counter
	led_pps_out <= not pps_in_sync;
	led_capt_out <= not capt_in_sync;

	-- align the counter value
	capt_count_byte_align <= std_logic_vector(resize(unsigned(capt_count), capt_count_byte_align'length));

	-- only drive the sdo pin (with spi_sdo) when chip_select is assreted (low)
	with ce_n_in select sdo_out <= spi_sdo when '0', 'Z' when others;

	-- spi instance
	i_spi_out: entity work.serial_out
	generic map(
		txwidth => capt_count_byte_align'length,
		--txwidth => capt_count'length,
		bmuxwidth => 5
	)
	port map(
		txreg_in => capt_count_byte_align,
		--txreg_in => capt_count,
		sclk_in => sclk_in,
		ce_n_in => ce_n_in,
		sdo_out => spi_sdo
	);
end behavioral;

