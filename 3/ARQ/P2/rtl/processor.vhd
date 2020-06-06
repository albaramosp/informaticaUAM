--------------------------------------------------------------------------------
-- Procesador MIPS con pipeline curso Arquitectura 2019-2020
-- 1363
-- Alba Ramos Pedroviejo
-- Ana Roa González
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity processor is
	port(
		Clk         : in  std_logic; -- Reloj activo en flanco subida
		Reset       : in  std_logic; -- Reset asincrono activo nivel alto

		-- Instruction memory
		IAddr      : out std_logic_vector(31 downto 0); -- Direccion Instr (sale del PC)
		IDataIn    : in  std_logic_vector(31 downto 0); -- Instruccion leida (sale de la memoria de instrucciones)

		-- Data memory
		DAddr      : out std_logic_vector(31 downto 0); -- Direccion
		DRdEn      : out std_logic;                     -- Habilitacion lectura
		DWrEn      : out std_logic;                     -- Habilitacion escritura
		DDataOut   : out std_logic_vector(31 downto 0); -- Dato escrito (entra a memoria, sale del banco)
		DDataIn    : in  std_logic_vector(31 downto 0)  -- Dato leido (sale de memoria, entra al banco)
	);
end processor;

architecture rtl of processor is

component reg_bank
	port (
		Clk : in std_logic; -- Reloj
		Reset : in std_logic; -- Reset asíncrono a nivel bajo
		A1 : in std_logic_vector(4 downto 0); -- registro a leer
		Rd1 : out std_logic_vector(31 downto 0); -- copia del valor de A1
		A2 : in std_logic_vector(4 downto 0); -- otro registro a leer
		Rd2 : out std_logic_vector(31 downto 0); -- copia del valor de A2
		A3 : in std_logic_vector(4 downto 0); -- registro sobre el que escribir
		--el dato Wd3
		Wd3 : in std_logic_vector(31 downto 0); -- Dato de entrada
		We3 : in std_logic -- Habilitación de escritura
	);
end component;

component alu is
	port (
		OpA     : in  std_logic_vector (31 downto 0); -- Operando A
		OpB     : in  std_logic_vector (31 downto 0); -- Operando B
		Control : in  std_logic_vector ( 3 downto 0); -- Codigo de control=op. a ejecutar
		Result  : out std_logic_vector (31 downto 0); -- Resultado
		ZFlag   : out std_logic                       -- Flag Z
	);
end component;

component alu_control is
	port (
		-- Entradas:
		ALUOp  : in std_logic_vector (2 downto 0); -- Codigo de control desde la unidad de control
		Funct  : in std_logic_vector (5 downto 0); -- Campo "funct" de la instruccion
		-- Salida de control para la ALU:
		ALUControl : out std_logic_vector (3 downto 0) -- Define operacion a ejecutar por la ALU
	);
end component;

component control_unit is
	port (
		-- Entrada = codigo de operacion en la instruccion:
		OpCode  : in  std_logic_vector (5 downto 0);

		-- Seniales para el PC
		Jump : out  std_logic;
		Branch : out  std_logic; -- 1 = Ejecutandose instruccion Branch_id
		-- Seniales relativas a la memoria
		MemtoReg : out  std_logic; -- 1 = Escribir en registro la salida de la mem.
		MemWrite : out  std_logic; -- Escribir la memoria
		MemRead  : out  std_logic; -- Leer la memoria
		-- Seniales para la ALU
		ALUSrc : out  std_logic;                     -- 0 = oper.B es registro, 1 = es valor inm.
		ALUOp  : out  std_logic_vector (2 downto 0); -- Tipo operacion para control de la ALU
		-- Seniales para el GPR
		RegWrite : out  std_logic; -- 1=Escribir registro
		RegDst   : out  std_logic  -- 0=Reg. destino es rt, 1=rd
	);
end component;

component forwarding_unit is
	port (
		id_ex_rs   		: in std_logic_vector(4 downto 0);
		id_ex_rt   		: in std_logic_vector(4 downto 0);
		ex_mem_rd   		: in std_logic_vector(4 downto 0);
		mem_wb_rd   		: in std_logic_vector(4 downto 0);

		RegWrite_mem 		: in std_logic;
		RegWrite_wb 		: in std_logic;

		enable_mux_aluOp1 : out std_logic_vector(1 downto 0);
		enable_mux_aluOp2 : out std_logic_vector(1 downto 0)
	);
end component;

component hazard_detection is
	port (
		id_ex_MemRd : in std_logic;
		
		id_ex_rt	: in std_logic_vector(4 downto 0);
		if_id_rt	: in std_logic_vector(4 downto 0);
		if_id_rs	: in std_logic_vector(4 downto 0);

		PCWrite 	: out std_logic;
		mux_nop 	: out std_logic;
		IF_IDWrite : out std_logic
	);
end component;


-- Declaración de señales auxiliares --
signal dato_inm: std_logic_vector(31 downto 0);
signal pc: std_logic_vector(31 downto 0);
signal pc_next: std_logic_vector(31 downto 0);
signal jta: std_logic_vector(31 downto 0);
signal pc_src1: std_logic_vector(31 downto 0);
signal s_OpA : std_logic_vector(31 downto 0);
signal s_OpB : std_logic_vector(31 downto 0);
signal s_OpB_forward : std_logic_vector(31 downto 0);
signal s_zero : std_logic_vector(31 downto 0);
signal PCSrc, PCSrc_def, Jump, Jump_aux : std_logic;
signal ALUControl: std_logic_vector(3 downto 0);

signal RegWrite_id, MemtoReg_id, Branch_id, MemWrite_id, MemRead_id, ALUSrc_id, RegDst_id: std_logic;
signal RegWrite_id_aux, MemtoReg_id_aux, Branch_id_aux, MemWrite_id_aux, MemRead_id_aux, ALUSrc_id_aux, RegDst_id_aux: std_logic;
signal RegWrite_ex, MemtoReg_ex, Branch_ex, MemWrite_ex, MemRead_ex, ALUSrc_ex, ZFlag_ex, RegDst_ex: std_logic;
signal RegWrite_mem, MemtoReg_mem, Branch_mem, MemWrite_mem, MemRead_mem, ZFlag_mem: std_logic;
signal RegWrite_wb, MemtoReg_wb: std_logic;

signal s_signo_mult_id : std_logic_vector(31 downto 0);
signal s_signo_mult_ex : std_logic_vector(31 downto 0);

signal AluOp_id, AluOp_id_aux: std_logic_vector(2 downto 0);
signal AluOp_ex: std_logic_vector(2 downto 0);

signal bta_result_ex: std_logic_vector(31 downto 0);
signal bta_result_mem: std_logic_vector(31 downto 0);

signal resultALU_ex: std_logic_vector(31 downto 0);
signal resultALU_mem: std_logic_vector(31 downto 0);
signal resultALU_wb: std_logic_vector(31 downto 0);

signal IDataIn_id: std_logic_vector(31 downto 0);
signal IDataIn_ex: std_logic_vector(31 downto 0);

signal DDataIn_wb: std_logic_vector(31 downto 0);

signal pc_plus4_if: std_logic_vector(31 downto 0);
signal pc_plus4_id: std_logic_vector(31 downto 0);
signal pc_plus4_ex: std_logic_vector(31 downto 0);

signal s_R1_id: std_logic_vector(31 downto 0);
signal s_R1_ex: std_logic_vector(31 downto 0);
signal s_R2_id: std_logic_vector(31 downto 0);
signal s_R2_ex: std_logic_vector(31 downto 0);
signal s_ddataout_mem: std_logic_vector(31 downto 0);
signal s_forward_ddataout_ex: std_logic_vector(31 downto 0);

signal s_signo_id : std_logic_vector(31 downto 0);
signal s_signo_ex : std_logic_vector(31 downto 0);


signal s_a3_ex : std_logic_vector(4 downto 0);
signal s_a3_mem : std_logic_vector(4 downto 0);
signal s_a3_wb : std_logic_vector(4 downto 0);
signal s_a3_wb_fw : std_logic_vector(4 downto 0);

signal s_wd3_wb : std_logic_vector(31 downto 0);

signal s_mux_aluOp1 : std_logic_vector(1 downto 0);
signal s_mux_aluOp2 : std_logic_vector(1 downto 0);

signal PCWrite, IF_IDWrite_hazard, IF_IDWrite, ID_EXWrite, EX_MEMWrite, mux_nop_hazard : std_logic;

begin
	DAddr <= resultALU_mem;
	DWrEn <= MemWrite_mem;
	DRdEn <= MemRead_mem;
	DDataOut <= s_ddataout_mem;
	s_a3_wb_fw <= s_a3_wb;
	s_forward_ddataout_ex <= s_OpB_forward;
			 

	--Creamos MUX
	s_opA <= s_R1_ex when s_mux_aluOp1 = "00" else
			 s_wd3_wb when s_mux_aluOp1 = "01" else
			 resultALU_mem;

	s_OpB_forward <= s_R2_ex when s_mux_aluOP2 = "00" else
					 s_wd3_wb when s_mux_aluOp2 = "01" else
					 resultALU_mem;

	s_OpB <= s_OpB_forward when ALUSrc_ex = '0' else
			 s_signo_ex;

	s_wd3_wb <= resultALU_wb when MemtoReg_wb = '0' else
			    DDataIn_wb(31 downto 0);

	s_a3_ex <= IDataIn_ex(20 downto 16) when RegDst_ex = '0' else
			   IDataIn_ex(15 downto 11);

	pc_src1 <= pc_plus4_if when PCSrc = '0' else
			   bta_result_mem;

	pc_next <= pc_src1 when  Jump = '0' else
			   jta;
	
	MemtoReg_id <= MemtoReg_id_aux when mux_nop_hazard = '0'
				   else '0';
				   
	MemWrite_id <= MemWrite_id_aux when mux_nop_hazard = '0'
				   else '0';
				   
	MemRead_id <= MemRead_id_aux when mux_nop_hazard = '0'
				   else '0';
				   
	Branch_id <= Branch_id_aux when mux_nop_hazard = '0'
				   else '0';
				   
	RegDst_id <= RegDst_id_aux when mux_nop_hazard = '0'
				   else '0';
				   
	ALUOp_id <= ALUOp_id_aux when mux_nop_hazard = '0'
				   else "000";
				   
	ALUSrc_id <= ALUSrc_id_aux when mux_nop_hazard = '0'
				   else '0';
	
	Jump <= Jump_aux when mux_nop_hazard = '0'
				   else '0';
				   
	RegWrite_id <= RegWrite_id_aux when mux_nop_hazard = '0'
				   else '0';
	
	IF_IDWrite <= '0' when PcSrc = '1'
				  else '1';
				  
	ID_EXWrite <= '0' when PcSrc = '1'
				  else '1';

	EX_MEMWrite <= '0' when PcSrc = '1'
				   else '1';		  
		  
   -- Instancia de unidad de adelantamiento
	miAdelantamiento: forwarding_unit
	port map (
	  id_ex_rs => IDataIn_ex(25 downto 21),
	  ex_mem_rd => s_a3_mem,
	  id_ex_rt => IDataIn_ex(20 downto 16),
	  mem_wb_rd => s_a3_wb_fw,
	  RegWrite_mem => RegWrite_mem,
	  RegWrite_wb => RegWrite_wb,
	  enable_mux_aluOp1 => s_mux_aluOp1,
	  enable_mux_aluOp2 => s_mux_aluOp2
	);

	-- Instancia de RegMIPS
	miRegistro: reg_bank
	port map(
		Clk => Clk,
		Reset => Reset,
		A1 =>IDataIn_id(25 downto 21),
		Rd1 => s_R1_id,
		A2 => IDataIn_id(20 downto 16),
		Rd2 => s_R2_id,
		A3 => s_a3_wb,
		Wd3 => s_wd3_wb,
		We3 => regWrite_wb
	);

	-- Instancia de alu_control
	miAluControl: alu_control
	port map(
		ALUOp => AluOp_ex,
		Funct => s_signo_ex(5 downto 0),
		ALUControl => ALUControl
	);

	--Instanciamos la UnidadControl
	miUnidaddecontrol: control_unit
	port map(
		OPCode => IDataIn_id(31 downto 26),
		Jump => Jump_aux,
		Branch => Branch_id_aux,
		MemtoReg => MemtoReg_id_aux,
		MemWrite => MemWrite_id_aux,
		MemRead => MemRead_id_aux,
		ALUOp => AluOp_id_aux,
		ALUSrc => AluSrc_id_aux,
		RegWrite => RegWrite_id_aux,
		RegDst  => RegDst_id_aux
	);

	-- Instancia de la alu
	miAlu: alu
	port map(
		OpA => s_opA,
		OpB => s_OpB,
		Control => ALUControl,
		Result => resultALU_ex,
		ZFlag => ZFlag_ex
	);
	
	-- Instancia de la unidad de deteccion de riesgos
	miHazard: hazard_detection
	port map (
		id_ex_MemRd => MemRead_ex,
		id_ex_rt => IDataIn_ex(20 downto 16),
		if_id_rt => IDataIn_id(20 downto 16),
		if_id_rs => IDataIn_id(25 downto 21),

		PCWrite => PCWrite,
		mux_nop => mux_nop_hazard,
		IF_IDWrite => IF_IDWrite_hazard
	);

	-- Extensión de signo del dato inmediato
	s_signo_id <= IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15 downto 0);
		
	--Creamos la and para PcSrc
	PCSrc <=  Branch_mem and ZFlag_mem;

	--Sumador del pc + 4
	pc_plus4_if <= pc + 4;

	--calculamos jta
	jta <= pc_plus4_id(31 downto 28)&IDataIn_id(25 downto 0)&"00";
	
	-- Multiplicar por 4 el signo extendido
	s_signo_mult_ex <= s_signo_ex(29 downto 0 )&"00";
	
	--Sumador del bta_result_ex
	bta_result_ex <= s_signo_mult_ex + pc_plus4_ex;


	IAddr <= pc;

	-- Ruta del PC
	process(Clk, Reset)
	begin
	  if Reset='1' then
	    pc <= (others => '0');
	  
	  elsif(rising_edge(Clk)) then
		if PCWrite='1' then
			pc <= pc_next;
		else
			pc <= pc;
		end if;
	  end if;
	end process;

	-- IF/ID
	process(Clk, Reset)
	begin
	  if Reset='1' then
		pc_plus4_id <= (others => '0');
		IDataIn_id <= (others => '0');
	  
	  elsif(rising_edge(Clk)) then
		if IF_IDWrite='1' then
			if IF_IDWrite_hazard='1' then
				pc_plus4_id <= pc_plus4_if;
				IDataIn_id <= IDataIn;
			else
				pc_plus4_id <= pc_plus4_id;
				IDataIn_id <= IDataIn_id;
			
			end if;
		else
			pc_plus4_id <= (others => '0');
			IDataIn_id <= (others => '0');
		end if;
	  end if;
	end process;

	-- ID/EX
	process(Clk, Reset)
	begin
	  if Reset='1' then
		pc_plus4_ex <= (others => '0');
		IDataIn_ex <= (others => '0');
		s_signo_ex <= (others => '0');
		s_R1_ex <= (others => '0');
		s_R2_ex <= (others => '0');
		RegDst_ex <= '0';
		Branch_ex <= '0';
		MemRead_ex <= '0';
		MemtoReg_ex <= '0';
		AluOp_ex <= (others => '0');
		MemWrite_ex <= '0';
		ALUSrc_ex <= '0';
		RegWrite_ex <= '0';
	  elsif(rising_edge(Clk)) then
		if ID_EXWrite='1' then
			pc_plus4_ex <= pc_plus4_id;
			s_signo_ex <= s_signo_id;
			IDataIn_ex <= IDataIn_id;
			s_R1_ex <= s_R1_id;
			s_R2_ex <= s_R2_id;
			RegDst_ex <= RegDst_id;
			Branch_ex <= Branch_id;
			MemRead_ex <= MemRead_id;
			MemtoReg_ex <= MemtoReg_id;
			AluOp_ex <= AluOp_id;
			MemWrite_ex <= MemWrite_id;
			ALUSrc_ex <= ALUSrc_id;
			RegWrite_ex <= RegWrite_id;
		else 
			pc_plus4_ex <= (others => '0');
			IDataIn_ex <= (others => '0');
			s_signo_ex <= (others => '0');
			s_R1_ex <= (others => '0');
			s_R2_ex <= (others => '0');
			RegDst_ex <= '0';
			Branch_ex <= '0';
			MemRead_ex <= '0';
			MemtoReg_ex <= '0';
			AluOp_ex <= (others => '0');
			MemWrite_ex <= '0';
			ALUSrc_ex <= '0';
			RegWrite_ex <= '0';
		end if;
	  end if;
	end process;


	-- EX/MEM
	process(Clk, Reset)
	begin
	  if Reset='1' then
		RegWrite_mem <= '0';
		MemtoReg_mem <= '0';
		Branch_mem <= '0';
		MemRead_mem <= '0';
		MemWrite_mem <= '0';
		bta_result_mem <= (others => '0');
		ZFlag_mem <= '0';
		resultALU_mem <= (others => '0');
		s_ddataout_mem <= (others => '0');
		s_a3_mem <= (others => '0');
	  elsif(rising_edge(Clk)) then
		if EX_MEMWrite='1' then
			RegWrite_mem <= RegWrite_ex;
			MemtoReg_mem <= MemtoReg_ex;
			Branch_mem <= Branch_ex;
			MemRead_mem <= MemRead_ex;
			MemWrite_mem <= MemWrite_ex;
			bta_result_mem <= bta_result_ex;
			ZFlag_mem <= ZFlag_ex;
			resultALU_mem <= resultALU_ex;
			s_ddataout_mem <= s_forward_ddataout_ex;
			s_a3_mem <= s_a3_ex;
		else 
			RegWrite_mem <= '0';
			MemtoReg_mem <= '0';
			Branch_mem <= '0';
			MemRead_mem <= '0';
			MemWrite_mem <= '0';
			bta_result_mem <= (others => '0');
			ZFlag_mem <= '0';
			resultALU_mem <= (others => '0');
			s_ddataout_mem <= (others => '0');
			s_a3_mem <= (others => '0');
		end if;
	  end if;
	end process;

	-- MEM/WB
	process(Clk, Reset)
	begin
	  if Reset='1' then
		RegWrite_wb <= '0';
		MemtoReg_wb <= '0';
		DDataIn_wb <= (others => '0');
		resultALU_wb <= (others => '0');
		s_a3_wb <= (others => '0');
	  elsif(rising_edge(Clk)) then
		RegWrite_wb <= RegWrite_mem;
		MemtoReg_wb <= MemtoReg_mem;
		DDataIn_wb <= DDataIn;
		resultALU_wb <= resultALU_mem;
		s_a3_wb <= s_a3_mem;
	  end if;
	end process;
end architecture;
