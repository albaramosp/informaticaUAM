--------------------------------------------------------------------------------
-- Bloque de control para la ALU. Arq0 2019-2020.
-- 1363
-- Alba Ramos Pedroviejo
-- Ana Roa González
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity alu_control is
   port (
      -- Entradas:
      ALUOp  : in std_logic_vector (2 downto 0); -- Codigo de control desde la unidad de control
      Funct  : in std_logic_vector (5 downto 0); -- Campo "funct" de la instruccion
      -- Salida de control para la ALU:
      ALUControl : out std_logic_vector (3 downto 0) -- Define operacion a ejecutar por la ALU
   );
end alu_control;

architecture rtl of alu_control is
begin

	ALUControl<= "0000" when ALUOp = "000" and Funct = "100000" else --add
				 "0100" when ALUOp = "000" and Funct = "100100" else --and
				 "0111" when ALUOp = "000" and Funct = "100101" else --or
				 "0001" when ALUOp = "000" and Funct = "100010" else --sub
				 "0110" when ALUOp = "000" and Funct = "100110" else --xor
				 "0000" when ALUOp = "001" else 					 --addi
				 "0000" when ALUOp = "010" else 					 --lw
				 "0000" when ALUOp = "011" else 					 --sw
				 "1010" when ALUOp = "100" else 					 --slti
				 "1101" when ALUOp = "101"	   		 			     --lui
				 else "0001"; --j y beq

end architecture;
