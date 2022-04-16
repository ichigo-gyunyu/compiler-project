	global main
	extern printf
	extern scanf

section .data
	informat: db "%ld",0
	outformat: db "%ld",10,0
	rinformat: db "%lf",0
	routformat: db "%lf",10,0

section .bss
	c2	        resq 1
	b5b567	        resq 1
	c3bd	        resq 1
	d3	        resq 1

section .text
main:
	; assign
	mov		rax, 1
	mov		[b5b567], rax

	; assign
	mov		rax, 0
	mov		[d3], rax

	; read
	push	rbx
	mov		rdi, informat
	mov		rsi, c2
	xor		rax, rax
	call	scanf
	pop		rbx

	; iter
c0:
	; precondition
	mov		rax, [b5b567]
	mov		rbx, [c2]
	cmp		rax, rbx
	jg		e0

i0:
	; iter stmts
	; read
	push	rbx
	mov		rdi, informat
	mov		rsi, c3bd
	xor		rax, rax
	call	scanf
	pop		rbx

	; assign
	mov		rax, [d3]
	push	rax
	mov		rbx, [c3bd]
	pop		rax
	add		rax, rbx
	mov		[d3], rax

	; assign
	mov		rax, [b5b567]
	push	rax
	mov		rbx, 1
	pop		rax
	add		rax, rbx
	mov		[b5b567], rax

	jmp		c0

e0:

	; rem part
	; write
	mov		rax, [d3]
	push	rbx
	mov		rdi, outformat
	mov		rsi, rax
	xor		rax, rax
	call	printf
	pop		rbx


	ret
