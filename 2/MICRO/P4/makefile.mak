all: p4a.exe p4b.exe p4c.exe p4ac.exe

p4a.exe: p4a.obj
    tlink /t p4a.obj

p4a.obj: p4a.asm
    tasm /zi p4a.asm
	
p4ac.exe: p4ac.obj
	tlink /t p4ac.obj

p4ac.obj: p4ac.asm
    tasm /zi p4ac.asm

p4b.exe: p4b.obj
    tlink /v p4b

p4b.obj: p4b.asm
    tasm /zi p4b.asm
	
p4c.exe: p4c.obj
    tlink /v p4c

p4c.obj: p4c.asm
    tasm /zi p4c.asm
	
clean:
    del *.obj
    del *.exe
    del *.map
    del *.com