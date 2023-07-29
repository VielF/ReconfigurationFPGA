library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity Gray_Code is
port(
	   i_CLK : in bit;
   	o_COUNTER: out std_logic_vector(3 downto 0));
end Gray_Code;

architecture arch_1 of Gray_Code is
	   signal w_CLK : bit := '0';
	   signal w_COUNTER : std_logic_vector(3 downto 0) := "0000";
	   signal w_CLK_COUNTER : std_logic_vector(24 downto 0) := (OTHERS => '0');
begin

	   u_0: process(w_CLK) begin
   		   if(w_CLK'event and w_CLK = '1') then
			         w_COUNTER <= w_COUNTER + 1;
	      	end if;
	   end process;

   	u_1: process(i_CLK) begin
   		   if(i_CLK'event and i_CLK = '1') then
			         w_CLK_COUNTER <= w_CLK_COUNTER + 1;
		      	   if(w_CLK_COUNTER = 22999999) then
				            w_CLK <= not w_CLK;
         				   w_CLK_COUNTER <= (OTHERS => '0');
			         end if;
		      end if;
	   end process;

	   o_COUNTER <= w_COUNTER(3) & (w_COUNTER(3) xor w_COUNTER(2)) & (w_COUNTER(2) xor
                w_COUNTER(1)) & (w_COUNTER(1) xor w_COUNTER(0));

end arch_1;
