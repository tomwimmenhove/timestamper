--------------------------------------------------------------------------------
-- company: 
-- engineer:
--
-- create date:   20:03:41 03/01/2019
-- design name:   
-- module name:   /home/tom/grive-tom.wimmenhove/projects/cosmic/hardware/capture_counter/tb_serial_out.vhd
-- project name:  capture_counter
-- target device:  
-- tool versions:  
-- description:   
-- 
-- vhdl test bench created by ise for module: serial_out
-- 
-- dependencies:
-- 
-- revision:
-- revision 0.01 - file created
-- additional comments:
--
-- notes: 
-- this testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  xilinx recommends
-- that these types always be used for the top-level i/o of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
 
-- uncomment the following library declaration if using
-- arithmetic functions with signed or unsigned values
--use ieee.numeric_std.all;
 
entity tb_serial_out is
end tb_serial_out;
 
architecture behavior of tb_serial_out is 
 
    -- component declaration for the unit under test (uut)
 
    component serial_out
    port(
         txreg_in : in  std_logic_vector(7 downto 0);
         sclk_in : in  std_logic;
         ce_n_in : in  std_logic;
         sdo_out : out  std_logic
        );
    end component;
    

   --inputs
   signal txreg : std_logic_vector(7 downto 0) := (others => '0');
   signal sclk : std_logic := '0';
   signal ce_n : std_logic := '0';

 	--outputs
   signal sdo : std_logic;
begin
 
	-- instantiate the unit under test (uut)
   uut: serial_out port map (
          txreg_in => txreg,
          sclk_in => sclk,
          ce_n_in => ce_n,
          sdo_out => sdo
        );

   -- stimulus process
   stim_proc: process
		variable i: integer;
   begin
		ce_n <= '1';
		wait for 10ns;
		
		txreg <= "10101010";
		ce_n <= '0';
		for i in 0 to 7 loop
			wait for 10ns;
			sclk <= '1';
			wait for 10ns;
			sclk <= '0';
		end loop;
		wait for 10ns;
		ce_n <= '1';
		
		wait for 20ns;

		txreg <= "01010101";
		ce_n <= '1';
		for i in 0 to 7 loop
			wait for 10ns;
			sclk <= '1';
			wait for 10ns;
			sclk <= '0';
		end loop;
		wait for 10ns;

		ce_n <= '0';
		
		wait for 10ns;
		for i in 0 to 7 loop
			wait for 10ns;
			sclk <= '1';
			wait for 10ns;
			sclk <= '0';
		end loop;

		ce_n <= '1';
		
		for i in 0 to 7 loop
			wait for 10ns;
			sclk <= '1';
			wait for 10ns;
			sclk <= '0';
		end loop;
		wait for 10ns;
		ce_n <= '1';


      wait;
   end process;

end;
