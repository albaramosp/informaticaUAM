.data
num0: .word 1 # posic 0
num1: .word 2 # posic 4
num2: .word 4 # posic 8 
num3: .word 8 # posic 12 
num4: .word 16 # posic 16 
num5: .word 32 # posic 20
num6: .word 0 # posic 24
num7: .word 0 # posic 28
num8: .word 0 # posic 32
num9: .word 0 # posic 36
num10: .word 0 # posic 40
num11: .word 0 # posic 44
.text
main:
  # carga num0 a num5 en los registros 9 a 14
  lw $t1, 0($zero) # lw $r9, 0($r0)
  lw $t2, 4($zero) # lw $r10, 4($r0)
  lw $t3, 8($zero) # lw $r11, 8($r0)
  lw $t4, 12($zero) # lw $r12, 12($r0)
  lw $t5, 16($zero) # lw $r13, 16($r0)
  lw $t6, 20($zero) # lw $r14, 20($r0)
  nop
  nop
  nop
  nop
  # SALTOS NORMALES
  add $t3, $t1, $t2 # en r11 un 3 = 1 + 2
  add $t1, $t3, $t2 # dependencia con la anterior # en r9 un 5 = 2 + 3
  add $t2, $t4, $t3 #dependencia con la anterior # en r10 un 11 = 3 + 8 
  nop
  nop
  nop
  beq $t3, $t3, T1 # salta
  add $t2, $t2, $t2 # En r10 un 22
  nop
  nop
  T1:add $t3, $t1, $t2 # en r11 un 16 = 5 + 11
  nop
  beq $t2, $t1, t2 # NO salta
  add $t2, $t2, $t2 # En r10 un 22
  nop
  nop
  t2:add $t3, $t1, $t2 # en r11 un 27 = 5 + 22
  beq $t3, $t3, t3 # salta
  nop
  nop
  t3:sub $t2, $t2, $t1 # en r10 un 17 = 22 - 5
  nop
  beq $t2, $t3, t4 # NO salta
  sub $t3, $t3, $t1 # en r11 un 22 = 27 - 5
  nop
  t4:sub $t2, $t2, $t1 # en r10 un 12 = 17 - 5
  lw $t1, 0($zero)  # en r9 un 1
  beq $t1, $t1, t5 # SALTA
  sub $t3, $t3, $t1 # en r11 un 21 = 22 - 1
  t5:sub $t1, $t1, $t1 # en r9 un 0
  nop
  nop
  lw $t1, 1($zero)  # en r9 un 1
  nop
  beq $t1, $t3, t7 # NO SALTA
  sub $t1, $t1, $t1 # en r9 un 0
  t7:sub $t2, $t2, $t2 # en r10 un 0
  