global jump_usermode
extern umain

section .text

jump_usermode:
	mov rcx, 0xc0000082
	wrmsr
	mov rcx, 0xc0000080
	rdmsr
	or eax, 1
	wrmsr
	mov rcx, 0xc0000081
	rdmsr
	mov edx, 0x00180008
	wrmsr

	mov ecx, umain
	mov r11, 0x202
	o64 sysret
