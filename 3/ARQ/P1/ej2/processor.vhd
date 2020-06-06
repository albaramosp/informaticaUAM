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


-- Declaración de señales auxiliares --
signal dato_inm: std_logic_vector(31 downto 0);
signal pc: std_logic_vector(31 downto 0);
signal pc_next: std_logic_vector(31 downto 0);
signal jta: std_logic_vector(31 downto 0);
signal pc_src1: std_logic_vector(31 downto 0); 
signal s_OpB : std_logic_vector(31 downto 0);
signal s_signo_mult_ex : std_logic_vector(31 downto 0);
signal s_zero : std_logic_vector(31 downto 0);
signal PCSrc, Jump : std_logic; 
signal ALUControl: std_logic_vector(3 downto 0);

signal RegWrite_id, MemtoReg_id, Branch_id, MemWrite_id, MemRead_id, ALUSrc_id, RegDst_id: std_logic;
signal RegWrite_ex, MemtoReg_ex, Branch_ex, MemWrite_ex, MemRead_ex, ALUSrc_ex, ZFlag_ex, RegDst_ex: std_logic;
signal RegWrite_mem, MemtoReg_mem, Branch_mem, MemWrite_mem, MemRead_mem, ZFlag_mem: std_logic;
signal RegWrite_wb, MemtoReg_wb: std_logic;
signal AluOp_id: std_logic_vector(2 downto 0);
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
signal s_R2_mem: std_logic_vector(31 downto 0);

signal s_signo_id : std_logic_vector(31 downto 0);
signal s_signo_ex : std_logic_vector(31 downto 0);


signal s_a3_ex : std_logic_vector(4 downto 0);
signal s_a3_mem : std_logic_vector(4 downto 0);
signal s_a3_wb : std_logic_vector(4 downto 0);

signal s_wd3_wb : std_logic_vector(31 downto 0);

begin   
	DAddr <= resultALU_mem;
	DWrEn <= MemWrite_mem;
	DRdEn <= MemRead_mem;
	DDataOut <= s_R2_mem;
	
	
	--Creamos la and para PcSrc
	PCSrc <=  Branch_mem and ZFlag_mem;

	--Creamos MUX
	s_OpB <= s_R2_ex when ALUSrc_ex = '0' else
			 s_signo_ex;

	s_wd3_wb <= resultALU_wb when MemtoReg_wb = '0' else
			 DDataIn_wb(31 downto 0);

	s_a3_ex <= IDataIn_ex(20 downto 16) when RegDst_ex = '0' else
			IDataIn_ex(15 downto 11);
			
	pc_src1 <= pc_plus4_if when PCSrc = '0' else 
			   bta_result_mem;
			
	pc_next <= pc_src1 when  Jump = '0' else
			   jta;
			   
			   
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
		Jump => Jump,
		Branch => Branch_id,
		MemtoReg => MemtoReg_id,
		MemWrite => MemWrite_id,
		MemRead => MemRead_id,
		ALUOp => AluOp_id,
		ALUSrc => AluSrc_id,
		RegWrite => RegWrite_id,
		RegDst  => RegDst_id
	);
	
	-- Instancia de la alu
	miAlu: alu
	port map(
		OpA => s_R1_ex,
		OpB => s_OpB,
		Control => ALUControl,
		Result => resultALU_ex,
		ZFlag => ZFlag_ex
	);
	
	-- Extensión de signo del dato inmediato
	s_signo_id <= IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15)&
		IDataIn_id(15)&IDataIn_id(15)&IDataIn_id(15 downto 0);
		
	-- Multiplicar por 4 el signo extendido
	s_signo_mult_ex <= s_signo_ex(29 downto 0 )&"00";
	
	--Sumador del pc + 4
	pc_plus4_if <= pc + 4;
	
	--Sumador del bta_result_ex
	bta_result_ex <= s_signo_mult_ex + pc_plus4_ex;
	
	--calculamos jta
	jta <= pc_plus4_id(31 downto 28)&IDataIn_id(25 downto 0)&"00";
	
	
	IAddr <= pc;
	
	-- Ruta del PC
	process(Clk, Reset)
	begin
	  if Reset='1' then
	    pc <= (others => '0');
	  elsif(rising_edge(Clk)) then
		pc <= pc_next;
	  end if; 
	end process;
	
	-- IF/ID
	process(Clk, Reset)
	begin
	  if Reset='1' then
		pc_plus4_id <= (others => '0');
		IDataIn_id <= (others => '0');
	  elsif(rising_edge(Clk)) then
		pc_plus4_id <= pc_plus4_if;
		IDataIn_id <= IDataIn;
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
		s_R2_mem <= (others => '0');
		s_a3_mem <= (others => '0');
	  elsif(rising_edge(Clk)) then
		RegWrite_mem <= RegWrite_ex;
		MemtoReg_mem <= MemtoReg_ex;
		Branch_mem <= Branch_ex;
		MemRead_mem <= MemRead_ex;
		MemWrite_mem <= MemWrite_ex;
		bta_result_mem <= bta_result_ex;
		ZFlag_mem <= ZFlag_ex;
		resultALU_mem <= resultALU_ex;
		s_R2_mem <= s_R2_ex;
		s_a3_mem <= s_a3_ex;
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
