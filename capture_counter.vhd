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
	
	TYPE Counter_State_Type IS (armed, holdoff);
	SIGNAL COUNTER_STATE : Counter_State_Type;
begin
	--CounterProcess: process(RST, CLK)
	CounterProcess: process(CLK)
	begin
		if rising_edge(CLK) then
			case COUNTER_STATE is
			
				when armed =>
					if RST = '1' then
						COUNTER <= (others => '0');
						COUNTER_STATE <= holdoff;
					else
						COUNTER <= COUNTER + 1;
						COUNTER_STATE <= armed;
					end if;
					
				when holdoff =>
					COUNTER <= COUNTER + 1;
					if RST = '0' then
						COUNTER_STATE <= armed;
					else
						COUNTER_STATE <= holdoff;
					end if;
									
			end case;
		end if;
	end process;
		
	--CaptureProcess: process(CAPT, RSTCAPT, CAPTURE_ENABLE, WAIT_RESET)
	CaptureProcess: process(CLK)
	begin
		if rising_edge(CLK) then
			if RSTCAPT = '1' then
				WAIT_RESET <= '0';
				INT <= '0';
			else
				--if CAPTURE_ENABLE = '1' and WAIT_RESET = '0' and rising_edge(CAPT) then
				if CAPTURE_ENABLE = '1' and WAIT_RESET = '0' and CAPT = '1' then
					LATCH <= COUNTER;
					WAIT_RESET <= '1';
					INT <= '1';
				end if;
			end if;
		end if;
	end process;
end Behavioral;

