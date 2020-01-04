;Move some data around. Wait 5 seconds then exit
;Will be killed by it's parent thread in the unit test before 5 seconds most likely

SYS_IN equ 0
SYS_EXIT equ 60
STDIN equ 0
SLEEP equ 35

%macro exit 1
    mov rax,SYS_EXIT
    mov rdi,%1
    syscall
%endmacro

section .data
	delay dq 5, 500000000

section .text 
	global _start

_start:
	;Do some work. This doesn't actually achieve anything
	mov r8,127
	mov [rsp-24],r8

	mov rax,SLEEP
	mov rdi,delay
	mov rsi,0
	syscall
	exit 0
