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
-- Description: Increase count on clk. Reset count on first clock with rst asserted.
--              Capture count into latch on first rising clock edge with capt asserted.
--              Capture disabled after capture until first rising clock edge with rstcap asserted.
--              Int is high when new data on the latch. Rstcap clears int.
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
		MRST_N: in std_logic;
	
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
	
	signal RST_ARMED: std_logic;
begin
	CounterProcess: process(MRST_N , CLK)
	begin
		if MRST_N = '0' then
			RST_ARMED <= '1';
			COUNTER <= (others => '0');
		else
			if rising_edge(CLK) then
				if RST_ARMED = '1' then
					if RST = '1' then
						COUNTER <= (others => '0');
						RST_ARMED <= '0';
					else
						COUNTER <= COUNTER + 1;
						RST_ARMED <= '1';
					end if;
				else
					COUNTER <= COUNTER + 1;
					if RST = '0' then
						RST_ARMED <= '1';
					else
						RST_ARMED <= '0';
					end if;
				end if;
			end if;
		end if;
	end process;
		
	CaptureProcess: process(MRST_N, CLK)
	begin
		if MRST_N = '0' then
			WAIT_RESET <= '0';
			INT <= '0';
			LATCH <= (others => '0');
		else
			if rising_edge(CLK) then
				if RSTCAPT = '1' then
					WAIT_RESET <= '0';
					INT <= '0';
				else
					if CAPTURE_ENABLE = '1' and WAIT_RESET = '0' and CAPT = '1' then
						LATCH <= COUNTER;
						WAIT_RESET <= '1';
						INT <= '1';
					end if;
				end if;
			end if;
		end if;
	end process;
end Behavioral;

