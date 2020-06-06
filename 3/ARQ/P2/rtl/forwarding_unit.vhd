--------------------------------------------------------------------------------
-- Universidad Autonoma de Madrid
-- Escuela Politecnica Superior
-- Laboratorio de Arq0 2019-2020
-- Alba Ramos Pedroviejo
-- Ana Roa González
-- Unidad de adelantamiento
----------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity forwarding_unit is
	port (
		id_ex_rs   			: in std_logic_vector(4 downto 0);
		id_ex_rt   			: in std_logic_vector(4 downto 0);
		ex_mem_rd   		: in std_logic_vector(4 downto 0);
		mem_wb_rd   		: in std_logic_vector(4 downto 0);

		RegWrite_mem 		: in std_logic;
		RegWrite_wb 		: in std_logic;

		enable_mux_aluOp1 	: out std_logic_vector(1 downto 0);
		enable_mux_aluOp2 	: out std_logic_vector(1 downto 0)
	);
end forwarding_unit;

architecture rtl of forwarding_unit is

begin

	enable_mux_aluOp1<=  "10" when RegWrite_mem = '1' and ex_mem_rd /= "00000" and ex_mem_rd = id_ex_rs else
						 "01" when RegWrite_wb = '1' and mem_wb_rd /= "00000" and mem_wb_rd = id_ex_rs
						 else "00";

	enable_mux_aluOp2<=	"10" when RegWrite_mem = '1' and ex_mem_rd /= "00000" and ex_mem_rd = id_ex_rt else
						"01" when RegWrite_wb = '1' and mem_wb_rd /= "00000" and mem_wb_rd = id_ex_rt
          				else "00";
end architecture;
