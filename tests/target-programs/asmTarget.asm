;Move some data around and call read from the terminal
;Equivilent of Cs getchar()
;Will be killed by it's parent thread in the unit test

SYS_IN equ 0
SYS_EXIT equ 60
STDIN equ 0

%macro exit 1
    mov rax,SYS_EXIT
    mov rdi,%1
    syscall
%endmacro

section .text 
	global _start

_start:
	mov r8,127
	mov [rsp-24],r8

	mov rax,SYS_IN
	mov rdi,STDIN
	mov rsi,5
	mov rdx,5
	syscall
	exit 0
