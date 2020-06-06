all: factor.exe
factor.exe: factor.obj
	tlink /v factor
factor.obj: factor.asm 
	tasm /zi factor.asm,,factor.lst