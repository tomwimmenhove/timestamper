----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:00:21 02/26/2019 
-- Design Name: 
-- Module Name:    capture_counter - Behavioral 
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

entity capture_counter is
	generic (
		WIDTH: integer := 8
	);
	 
	port (
		CAPTURE_ENABLE: in std_logic;	
		
		CLK: in std_logic;
		RST: in std_logic;
		
		CAPT: in std_logic;
		RSTCAPT: in std_logic;
		
		LATCH: out std_logic_vector(WIDTH - 1 downto 0);
		INT: out std_logic
	);
end capture_counter;

architecture Behavioral of capture_counter is
	signal COUNTER: std_logic_vector(WIDTH - 1 downto 0);
	signal WAIT_RESET: std_logic;
begin
	CounterProcess: process(RST, CLK)
	begin
		if RST = '1' then
			COUNTER <= (others => '0');
		else
			if rising_edge(CLK) then
				COUNTER <= COUNTER + 1;
			end if;
		end if;
	end process;
		
	CaptureProcess: process(CAPT, RSTCAPT, CAPTURE_ENABLE, WAIT_RESET)
	begin
		if RSTCAPT = '1' then
			WAIT_RESET <= '0';
		else
			if CAPTURE_ENABLE = '1' and WAIT_RESET = '0' and rising_edge(CAPT) then
				LATCH <= COUNTER;
				WAIT_RESET <= '1';
			end if;
		end if;
		
		INT <= WAIT_RESET;
	end process;
end Behavioral;

