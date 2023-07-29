library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity Pulse_Gen is
port(
   	i_CLK : in bit;
	  o_PULSE : out std_logic);
end Pulse_Gen;

architecture arch_1 of Pulse_Gen is
	   signal w_CLK_COUNTER : std_logic_vector(24 downto 0) := (OTHERS => '0');
	   signal w_PULSE : std_logic := '0';
begin

	u_0: process(i_CLK) begin
		   if(i_CLK'event and i_CLK = '1') then
			      w_CLK_COUNTER <= w_CLK_COUNTER + 1;
			      if(w_CLK_COUNTER = 24999998) then
				         w_PULSE <= not(w_PULSE);
				         w_CLK_COUNTER <= (OTHERS => '0');
			      end if;
		   end if;
	end process;

	o_PULSE <= w_PULSE;

end arch_1;
