----------------------------------------------------------------------------------
--
-- This file is part of the TimeStamper distribution (https://github.com/tomwimmenhove/timestamper)
-- Copyright (c) 2019 Tom wimmenhove
-- 
-- This program is free software: you can redistribute it and/or modify  
-- it under the terms of the GNU General Public License as published by  
-- the Free Software Foundation, version 3.
--
-- This program is distributed in the hope that it will be useful, but 
-- WITHOUT ANY WARRANTY; without even the implied warranty of 
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
-- General Public License for more details.
--
-- You should have received a copy of the GNU General Public License 
-- along with this program. If not, see <http://www.gnu.org/licenses/>.
--
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity serial_out is
	generic (
		txwidth: integer := 8;
		bmuxwidth: integer := 3
	);
	
	port (
		txreg_in: in std_logic_vector(txwidth - 1 downto 0);
		sclk_in: in std_logic;
		ce_n_in: in std_logic;
		sdo_out: out std_logic
	);
end serial_out;

architecture behavioral of serial_out is
	signal bmux: std_logic_vector(bmuxwidth - 1 downto 0);
begin
	spioutprocess: process(sclk_in, ce_n_in, bmux)
	begin
		if ce_n_in = '1' then
			bmux <= (others => '0');
		else
			if falling_edge(sclk_in) then -- cpol == cpha == 0
				bmux <= bmux + 1;
			end if;
		end if;
		
		if to_integer(unsigned(bmux)) < txwidth then
			sdo_out <= txreg_in(txwidth - to_integer(unsigned(bmux)) - 1);
		end if;
	end process;
end behavioral;
