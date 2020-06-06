--------------------------------------------------------------------------------
-- Universidad Autonoma de Madrid
-- Escuela Politecnica Superior
-- Laboratorio de Arq0 2019-2020
-- Alba Ramos Pedroviejo
-- Ana Roa González
-- Unidad de detección de riesgos para LW
----------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity hazard_detection is
	port (
		id_ex_MemRd : in std_logic;
		
		id_ex_rt	: in std_logic_vector(4 downto 0);
		if_id_rt	: in std_logic_vector(4 downto 0);
		if_id_rs	: in std_logic_vector(4 downto 0);

		PCWrite 	: out std_logic;
		mux_nop 	: out std_logic;
		IF_IDWrite : out std_logic
	);
end hazard_detection;

architecture rtl of hazard_detection is

	signal aux : std_logic := '0';

begin

	aux <= '0' when id_ex_MemRd = '1' and (id_ex_rt = if_id_rs or id_ex_rt = if_id_rt)
			   else '1';
	
	PCWrite <= '0' when aux = '0'
			    else '1';
				
	IF_IDWrite <= '0' when aux = '0'
				  else '1';
			
	mux_nop <= '1' when aux = '0'
				else '0';
end architecture;