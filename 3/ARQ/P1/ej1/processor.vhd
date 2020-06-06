--------------------------------------------------------------------------------
-- Procesador MIPS uniciclo curso Arquitectura 2019-2020
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
	  Branch : out  std_logic; -- 1 = Ejecutandose instruccion branch
      -- Seniales relativas a la memoria
      MemToReg : out  std_logic; -- 1 = Escribir en registro la salida de la mem.
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
signal s_R1: std_logic_vector(31 downto 0);
signal s_R2: std_logic_vector(31 downto 0);
signal resultALU: std_logic_vector(31 downto 0);
signal pc: std_logic_vector(31 downto 0);
signal pc_next: std_logic_vector(31 downto 0);
signal bta: std_logic_vector(31 downto 0);
signal jta: std_logic_vector(31 downto 0);
signal pc_plus4: std_logic_vector(31 downto 0);
signal pc_src1: std_logic_vector(31 downto 0);
signal s_mem_addr : std_logic_vector(31 downto 0);
signal s_mem_data : std_logic_vector(31 downto 0);
signal s_mem_addr_data : std_logic_vector(31 downto 0);
signal s_mem_data_data : std_logic_vector(31 downto 0);
signal s_mem_read_data : std_logic_vector(31 downto 0);
signal s_a3 : std_logic_vector(4 downto 0);
signal s_a3_src1 : std_logic_vector(4 downto 0);
signal s_a3_src2 : std_logic_vector(4 downto 0);
signal s_wd3 : std_logic_vector(31 downto 0);
signal s_OpB : std_logic_vector(31 downto 0);
signal s_signo : std_logic_vector(31 downto 0);
signal s_signo_mult : std_logic_vector(31 downto 0);
signal s_zero : std_logic_vector(31 downto 0);
signal s_AluOp: std_logic_vector(2 downto 0);
signal PCSrc, regWrite, ZFlag, MemtoReg, MemWrite, MemRead, Branch, AluSrc, RegDst, Jump : std_logic;
signal ALUControl: std_logic_vector(3 downto 0);

begin
	DAddr <= resultALU;
	DWrEn <= MemWrite;
	DRdEn <= MemRead;
	DDataOut <= s_R2;

	--Creamos la and para PcSrc
	PCSrc <=  Branch and ZFlag;

	--Creamos MUX
	s_OpB <= s_R2 when ALUSrc = '0' else
			 s_signo;

	s_wd3 <= resultALU when MemToReg = '0' else
			 DDataIn (31 downto 0);

	s_a3 <= IDataIn(20 downto 16) when RegDst = '0' else
			IDataIn(15 downto 11);

	pc_src1 <= pc_plus4 when PCSrc = '0' else
			   bta;

	pc_next <= pc_src1 when  Jump = '0' else
			   jta;


	-- Instancia de RegMIPS
	miRegistro: reg_bank
	port map(
		Clk => Clk,
		Reset => Reset,
		A1 =>IDataIn(25 downto 21),
		Rd1 => s_R1,
		A2 => IDataIn(20 downto 16),
		Rd2 => s_R2,
		A3 => s_a3,
		Wd3 => s_wd3,
		We3 => RegWrite
	);

	-- Instancia de alu_control
	miAluControl: alu_control
	port map(
		ALUOp => s_AluOp,
		Funct => IDataIn(5 downto 0),
		ALUControl => ALUControl
	);

	--Instanciamos la UnidadControl
	miUnidaddecontrol: control_unit
	port map(
		OPCode => IDataIn(31 downto 26),
		Jump => Jump,
		Branch => Branch,
		MemToReg => MemToReg,
		MemWrite => MemWrite,
		MemRead => MemRead,
		ALUOp => s_AluOp,
		ALUSrc => AluSrc,
		RegWrite => RegWrite,
		RegDst  => RegDst
	);

	-- Instancia de la alu
	miAlu: alu
	port map(
		OpA => s_R1,
		OpB => s_OpB,
		Control => ALUControl,
		Result => resultALU,
		ZFlag => ZFlag
	);

	-- Extensión de signo del dato inmediato
	s_signo <= IDataIn(15)&IDataIn(15)&IDataIn(15)&IDataIn(15)&
		IDataIn(15)&IDataIn(15)&IDataIn(15)&IDataIn(15)&IDataIn(15)&
		IDataIn(15)&IDataIn(15)&IDataIn(15)&IDataIn(15)&IDataIn(15)&
		IDataIn(15)&IDataIn(15)&IDataIn(15 downto 0);

	-- Multiplicar por 4 el signo extendido
	s_signo_mult <= s_signo(29 downto 0 )&"00";

	--Sumador del pc + 4
	pc_plus4 <= pc + 4;

	--Sumador del bta
	bta <= s_signo_mult + pc_plus4;

	--calculamos jta
	jta <= pc_plus4(31 downto 28)&IDataIn(25 downto 0)&"00";

	-- Ruta del PC
	process(Clk, Reset)
	begin
	  if Reset='1' then
	    pc <= (others => '0');
	  elsif(rising_edge(Clk)) then
	   pc <= pc_next;
	  end if;
	end process;

	IAddr <= pc;
end architecture;
