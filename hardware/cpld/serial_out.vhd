----------------------------------------------------------------------------------
-- company: 
-- engineer: Tom Wimmenhove
-- 
-- create date:    18:29:33 02/26/2019 
-- design name: 
-- module name:    serial_out - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: Serially clock out the contents of txreg
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
