----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:48:25 02/24/2019 
-- Design Name: 
-- Module Name:    main - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;
entity main is
	generic (
		--WIDTH: integer := 4
		WIDTH: integer := 27
	);
	 
	port (
		-- Counter
		CLK: in std_logic;
		RST: in std_logic;
		-- Counter capture
		CAPT: in std_logic;
		RSTCAPT: in std_logic;
			
		-- SPI output
		SCLK: in std_logic;
		CE_N: in std_logic;
		SDO: out std_logic;
		
		-- '1' when there's captured data
		INT: out std_logic
	);
end main;

architecture Behavioral of main is
	signal COUNT_LATCH: std_logic_vector(WIDTH - 1 downto 0);	-- The counter (capture) latch
	signal COUNT_BYTE_LATCH: std_logic_vector((WIDTH + (((8 - (WIDTH mod 8)) mod 8))) - 1 downto 0);	-- Left-justified byte-aligned representation of the counter latch
	
	signal SPI_SDO: std_logic;
begin
	-- Counter instance
	counter: entity work.capture_counter
	generic map(
		WIDTH => WIDTH
	)
	port map(
		CAPTURE_ENABLE => CE_N,
		CLK => CLK,
		RST => RST,
		CAPT => CAPT,
		RSTCAPT => RSTCAPT,
		LATCH => COUNT_LATCH,
		INT => INT
	);
	
	-- Align the counter value
	COUNT_BYTE_LATCH <= std_logic_vector(resize(unsigned(COUNT_LATCH), COUNT_BYTE_LATCH'length));

	-- Only drive the SDO pin (with SPI_SDO) when chip_select is assreted (low)
	with CE_N select SDO <= SPI_SDO when '0', 'Z' when others;

	-- SPI instance
	spi: entity work.serial_out
	generic map(
		TXWIDTH => COUNT_BYTE_LATCH'length
	)
	port map(
		TXREG => COUNT_BYTE_LATCH,
		SCLK => SCLK,
		CE_N => CE_N,
		SDO => SPI_SDO
	);
end Behavioral;

