

segment .bss
	__esp   resd 1
	_x  resd 1

segment .data
	_div_zero db "****Error de ejecucion: Division por cero.", 0
	_err_out_range db "****Error de ejecucion: Indice fuera de rango.", 0

segment .text
	global main
	extern print_string, print_endofline, print_blank, scan_int, print_int, print_boolean, scan_boolean

_func:
	push ebp
	mov ebp, esp
	sub esp, 0

	push dword 1

	pop dword eax
	mov esp, ebp
	pop ebp
	ret

main:
	mov dword [__esp], esp
	call _func
	add esp, 0
	push dword eax
	pop dword eax
	mov [_x], eax

	push dword _x

	pop dword eax
	mov eax, [eax]
	push dword eax
	call print_int
	add esp, 4
	call print_endofline

	jmp _fin

_err_div_0:
	push dword _div_zero
	call print_string
	call print_endofline
	add esp, 4
	jmp _fin

_err_vector_range:
	push dword _err_out_range
	call print_string
	call print_endofline
	add esp, 4
	jmp _fin

_fin:
	mov dword esp, [__esp]
	ret